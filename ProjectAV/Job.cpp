// Job.cpp

#include "Job.h"
#include "Step.h"
#include "Drawable.h"
#include "Graphics.h"

#include "Node.h"
#include "AnimationComponent.h"
#include "BoneTransformCbuf.h"  // For Bind::BoneTransformCbuf
#include "TransformCbuf.h"    // For Bind::TransformCbuf (if explicit update was needed)
// #include "PixelShader.h" // Not directly used in Job::Execute, probably leftover

// Ensure MAX_SHADER_BONES_CB is defined (e.g., from BoneTransformCbuf.h or a global config)
// If it's in BoneTransformCbuf.h, that include should be enough.
// extern const int MAX_SHADER_BONES_CB; // If it's a global const defined elsewhere.
// Or, preferably, get it from BoneTransformCbuf if it stores its capacity.

Job::Job(const Step* pStep_in, const Drawable* pDrawable_in)
    : pDrawable{ pDrawable_in },
    pStep{ pStep_in }
{
}

void Job::Execute(Graphics& gfx) const noxnd // Job::Execute is const
{
    if (!pStep || !pDrawable) {
        // Consider logging with more context if possible, e.g., from where this Job was created.
        return;
    }

    // --- UPDATE DYNAMIC CONSTANT BUFFERS ---
    const Node* ownerNode = pDrawable->GetParentNode(); // Assumes Drawable has this

    // Iterate through the Step's bindables to find and update specific CBs.
    // pStep is const, so GetBindables() should be a const method.
    // The shared_ptr itself is const-iterated, but the pointed-to Bindable can be non-const.
    for (const auto& bindableSharedPtr : pStep->GetBindables()) // Step must have GetBindables()
    {
        Bind::Bindable* bindableRawPtr = bindableSharedPtr.get();

        // Update TransformCbuf:
        // Your TransformCbuf has InitializeParentReference(*pDrawable) and its Bind() calls UpdateBindImpl.
        // So, when pStep->Bind() is called later, and it iterates to bind TransformCbuf,
        // TransformCbuf's own Bind() method will handle its update.
        // No explicit update for TransformCbuf is needed here.

        // Update BoneTransformCbuf if present in this Step
        if (auto* boneCbuf = dynamic_cast<Bind::BoneTransformCbuf*>(bindableRawPtr)) {
            if (ownerNode) {
                if (AnimationComponent* animComp = ownerNode->GetComponent<AnimationComponent>()) {
                    const auto& matrices = animComp->GetFinalBoneMatrices();
                    if (!matrices.empty()) {
                        boneCbuf->Update(gfx, matrices);
                    }
                    else if (animComp->GetBoneCount() > 0) {
                        // AnimComp exists, has bones, but matrices are empty (e.g. first frame, or anim stopped without setting bind pose)
                        // Send identity matrices.
                        static std::vector<DirectX::XMFLOAT4X4> identityMatrices; // Static to avoid re-allocation
                        int maxBonesForIdentity = MAX_SHADER_BONES_CB; // Use the CB's capacity
                        // Or, if BoneTransformCbuf could expose its capacity: maxBonesForIdentity = boneCbuf->GetMaxBones();

                        // Resize only if necessary
                        if (identityMatrices.size() != (size_t)maxBonesForIdentity) {
                            identityMatrices.resize(maxBonesForIdentity);
                            for (int i = 0; i < maxBonesForIdentity; ++i) {
                                DirectX::XMStoreFloat4x4(&identityMatrices[i], DirectX::XMMatrixIdentity());
                            }
                        }
                        boneCbuf->Update(gfx, identityMatrices);
                    }
                    // If animComp->GetBoneCount() is 0, matrices is empty, nothing to update for this animComp.
                    // The cbuffer will retain its last state or default (hopefully identity).
                }
                else {
                    // No AnimationComponent on this Node, but Step expects BoneTransformCbuf.
                    // This means the "Skinned" technique was chosen for a non-animated object (or one without AnimComp).
                    // Send default identity matrices to avoid using stale data.
                    static std::vector<DirectX::XMFLOAT4X4> defaultIdentityMatrices; // Static
                    if (defaultIdentityMatrices.size() != MAX_SHADER_BONES_CB) { // Check against actual CB capacity
                        defaultIdentityMatrices.resize(MAX_SHADER_BONES_CB);
                        for (int i = 0; i < MAX_SHADER_BONES_CB; ++i) {
                            DirectX::XMStoreFloat4x4(&defaultIdentityMatrices[i], DirectX::XMMatrixIdentity());
                        }
                    }
                    boneCbuf->Update(gfx, defaultIdentityMatrices);
                }
            }
            else { // No ownerNode for Drawable, this is problematic.
                // OutputDebugStringA("Job::Execute Warning: Drawable has no ownerNode, cannot update BoneTransformCbuf correctly.\n");
                // Still update with identity to be safe.
                static std::vector<DirectX::XMFLOAT4X4> errorIdentityMatrices; // Static
                if (errorIdentityMatrices.size() != MAX_SHADER_BONES_CB) {
                    errorIdentityMatrices.resize(MAX_SHADER_BONES_CB);
                    for (int i = 0; i < MAX_SHADER_BONES_CB; ++i) {
                        DirectX::XMStoreFloat4x4(&errorIdentityMatrices[i], DirectX::XMMatrixIdentity());
                    }
                }
                boneCbuf->Update(gfx, errorIdentityMatrices);
            }
        }
        // Add 'else if' for other dynamically updated CBs here if any.
    }

    // Bind Drawable's specific geometry (VB, IB, Topology)
    pDrawable->Bind(gfx);

    // Bind all of Step's pipeline states and resources (Shaders, updated CBs, Textures, Samplers, Rasterizer, Blender)
    pStep->Bind(gfx);

    // Execute the draw call
    gfx.DrawIndexed(pDrawable->GetIndexCount());
}
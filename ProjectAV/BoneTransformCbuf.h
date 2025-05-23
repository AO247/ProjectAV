// BoneTransformCbuf.h
#pragma once
#include "Bindable.h" // Your base Bindable class
#include "Graphics.h"
#include <DirectXMath.h>
#include <vector>
// #include "AnimationComponent.h" // For MAX_ANIM_COMPONENT_BONES, or define it globally

// Define this constant where it's globally accessible, or pass it in.
// For now, let's assume it's accessible here.
// If AnimationComponent::MAX_ANIM_COMPONENT_BONES is private static, you'll need a getter or another way.
// Let's assume a global or accessible const:
const int MAX_SHADER_BONES_CB = 128; // Must match shader array size

namespace Bind
{
    class BoneTransformCbuf : public Bindable
    {
    public:
        // Constructor takes the graphics interface and the constant buffer slot
        BoneTransformCbuf(Graphics& gfx, UINT slot = 3u); // Default to slot b3

        // Updates the constant buffer with new bone matrices
        void Update(Graphics& gfx, const std::vector<DirectX::XMFLOAT4X4>& boneMatrices);

        void Bind(Graphics& gfx) noexcept override;

        // Static Resolve function for Codex (optional, but good pattern)
        static std::shared_ptr<BoneTransformCbuf> Resolve(Graphics& gfx, UINT slot = 3u);
        static std::string GenerateUID(UINT slot);
        std::string GetUID() const noexcept override { return GenerateUID(m_slot); }


    private:
        struct Transforms // This structure is mapped directly to the GPU CB
        {
            // HLSL matrices are column-major by default in cbuffers after C++ XMMatrixTranspose
            DirectX::XMFLOAT4X4 boneMatrices[MAX_SHADER_BONES_CB];
        };

        Microsoft::WRL::ComPtr<ID3D11Buffer> m_pConstantBuffer;
        UINT m_slot; // The slot this cbuffer binds to (e.g., 3 for b3)
    };
}
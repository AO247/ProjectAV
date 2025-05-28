// BoneTransformCbuf.h
#pragma once
#include "Bindable.h"
#include "Graphics.h"
#include <DirectXMath.h>
#include <vector>

const int MAX_SHADER_BONES_CB = 128; // Must match shader array size

namespace Bind
{
    class BoneTransformCbuf : public Bindable
    {
    public:
        BoneTransformCbuf(Graphics& gfx, UINT slot = 3u); // slot for b-register

        void Update(Graphics& gfx, const std::vector<DirectX::XMFLOAT4X4>& boneMatrices);
        void Bind(Graphics& gfx) noexcept override;

        static std::shared_ptr<BoneTransformCbuf> Resolve(Graphics& gfx, UINT slot = 3u);
        static std::string GenerateUID(UINT slot);
        std::string GetUID() const noexcept override { return GenerateUID(m_slot); }

    private:
        struct Transforms // Mapped directly to GPU CB
        {
            // Matrices here are expected to be column-major if shader is default HLSL
            // Your C++ code does: XMStoreFloat4x4(&mFinalBoneMatrices[id], XMMatrixTranspose(...))
            // So they are correctly prepared.
            DirectX::XMFLOAT4X4 boneMatrices[MAX_SHADER_BONES_CB];
        };

        Microsoft::WRL::ComPtr<ID3D11Buffer> m_pConstantBuffer;
        UINT m_slot;
    };
}
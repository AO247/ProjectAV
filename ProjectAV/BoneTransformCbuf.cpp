// BoneTransformCbuf.cpp
#include "BoneTransformCbuf.h"
#include "GraphicsThrowMacros.h" // For GFX_THROW_INFO, GetDevice, GetContext
#include "BindableCodex.h"       // For Codex::Resolve if using it

namespace Bind
{
    BoneTransformCbuf::BoneTransformCbuf(Graphics& gfx, UINT slot)
        : m_slot(slot)
    {
        INFOMAN(gfx);

        D3D11_BUFFER_DESC cbd = {};
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.Usage = D3D11_USAGE_DYNAMIC; // We'll update it frequently
        cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        cbd.MiscFlags = 0u;
        cbd.ByteWidth = sizeof(Transforms); // Size of our struct
        cbd.StructureByteStride = 0u;

        GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &m_pConstantBuffer));
    }

    void BoneTransformCbuf::Update(Graphics& gfx, const std::vector<DirectX::XMFLOAT4X4>& finalBoneMatricesData)
    {
        INFOMAN(gfx);

        Transforms transformsGpuData = {}; // Initialize to zero or default

        size_t countToCopy = std::min(finalBoneMatricesData.size(), (size_t)MAX_SHADER_BONES_CB);
        for (size_t i = 0; i < countToCopy; ++i)
        {
            // finalBoneMatricesData from AnimationComponent should ALREADY BE TRANSPOSED
            // if your shader expects column-major and they were calculated as row-major products.
            transformsGpuData.boneMatrices[i] = finalBoneMatricesData[i];
        }
        // Any remaining matrices in transformsGpuData.boneMatrices will be identity if not enough bones
        // (assuming Transforms constructor zero-initializes or you manually init to identity for unused slots)
        if (countToCopy < MAX_SHADER_BONES_CB) {
            for (size_t i = countToCopy; i < MAX_SHADER_BONES_CB; ++i) {
                DirectX::XMStoreFloat4x4(&transformsGpuData.boneMatrices[i], DirectX::XMMatrixIdentity());
            }
        }


        D3D11_MAPPED_SUBRESOURCE msr;
        GFX_THROW_INFO(GetContext(gfx)->Map(m_pConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));
        memcpy(msr.pData, &transformsGpuData, sizeof(Transforms));
        GetContext(gfx)->Unmap(m_pConstantBuffer.Get(), 0u);
    }

    void BoneTransformCbuf::Bind(Graphics& gfx) noexcept
    {
        GetContext(gfx)->VSSetConstantBuffers(m_slot, 1u, m_pConstantBuffer.GetAddressOf());
        // If also used in Pixel Shader (unlikely for bone matrices):
        // GetContext(gfx)->PSSetConstantBuffers(m_slot, 1u, m_pConstantBuffer.GetAddressOf());
    }

    std::shared_ptr<BoneTransformCbuf> BoneTransformCbuf::Resolve(Graphics& gfx, UINT slot)
    {
        return Codex::Resolve<BoneTransformCbuf>(gfx, slot);
    }

    std::string BoneTransformCbuf::GenerateUID(UINT slot)
    {
        using namespace std::string_literals;
        return typeid(BoneTransformCbuf).name() + "#s"s + std::to_string(slot);
    }

} // namespace Bind
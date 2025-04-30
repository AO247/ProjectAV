#include "SolidBox.h"
#include "BindableCommon.h"      
#include "GraphicsThrowMacros.h" 
#include "Vertex.h"              
#include "Box.h"                // Include the Box mesh generator
#include "Bindable.h"            
#include <memory>                
#include <string>                
#include <sstream>               

namespace { // Anonymous namespace for internal linkage
    // Keep VecToString for tag generation (using size now)
    inline std::string VecToString(const DirectX::XMFLOAT3& v) {
        std::ostringstream oss;
        oss << std::fixed << v.x << "_" << v.y << "_" << v.z;
        return oss.str();
    }
}

// --- Local BlendState Definition (Inside SolidBox.cpp) ---
// (Keep the SolidBoxBlendState definition as it was)
namespace Bind {
    class SolidBoxBlendState : public Bindable {
        // ... (Blend state implementation remains the same) ...
    public:
        SolidBoxBlendState(Graphics& gfx) {
            INFOMAN(gfx);
            D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC{ CD3D11_DEFAULT{} };
            auto& brt = blendDesc.RenderTarget[0];
            brt.BlendEnable = TRUE;
            brt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
            brt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            brt.BlendOp = D3D11_BLEND_OP_ADD;
            brt.SrcBlendAlpha = D3D11_BLEND_ONE;
            brt.DestBlendAlpha = D3D11_BLEND_ZERO;
            brt.BlendOpAlpha = D3D11_BLEND_OP_ADD;
            brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            GFX_THROW_INFO(GetDevice(gfx)->CreateBlendState(&blendDesc, &pBlender));
        }
        void Bind(Graphics& gfx) noexcept override {
            GetContext(gfx)->OMSetBlendState(pBlender.Get(), nullptr, 0xFFFFFFFFu);
        }
    protected:
        Microsoft::WRL::ComPtr<ID3D11BlendState> pBlender;
    };
} // namespace Bind
// --- End Local BlendState Definition ---


// --- **** UPDATED: SolidBox Constructor uses Initialize **** ---
SolidBox::SolidBox(Graphics& gfx, DirectX::XMFLOAT3 center, DirectX::XMFLOAT3 size)
{
    DirectX::XMStoreFloat4x4(&matrix, DirectX::XMMatrixIdentity()); // Initialize matrix member
    Initialize(gfx, center, size);
}

// --- **** UPDATED: SolidBox Initialize uses center and size **** ---
void SolidBox::Initialize(Graphics& gfx, DirectX::XMFLOAT3 center, DirectX::XMFLOAT3 size)
{
    using namespace Bind;
    namespace dx = DirectX;

    // --- Calculate min/max extents from center and size ---
    DirectX::XMFLOAT3 halfSize = { size.x / 2.0f, size.y / 2.0f, size.z / 2.0f };
    DirectX::XMFLOAT3 minExtents = { center.x - halfSize.x, center.y - halfSize.y, center.z - halfSize.z };
    DirectX::XMFLOAT3 maxExtents = { center.x + halfSize.x, center.y + halfSize.y, center.z + halfSize.z };
    // --- End Calculation ---

    // Generate the AABB mesh data using calculated extents
    auto model = Box::Make(minExtents, maxExtents); // Box::Make still uses min/max

    // **** UPDATED: Create geometry tag based on SIZE **** 
    // (This assumes the underlying geometry depends only on size, not center offset)
    const auto geometryTag = "$box." + VecToString(size);

    // Resolve shared bindables via Codex
    AddBind(VertexBuffer::Resolve(gfx, geometryTag, model.vertices));
    AddBind(IndexBuffer::Resolve(gfx, geometryTag, model.indices));
    AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

    // Resolve shared shaders and input layout via Codex
    auto pvs = VertexShader::Resolve(gfx, "SolidVS.cso");
    auto pvsbc = pvs->GetBytecode();
    AddBind(std::move(pvs));

    AddBind(PixelShader::Resolve(gfx, "SolidPS.cso"));
    AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

    // Constant Buffer for Color+Alpha
    struct PSColorConstant { dx::XMFLOAT4 color; } colorConst;
    static_assert((sizeof(PSColorConstant) % 16) == 0, "PSColorConstant size must be multiple of 16 bytes");
    colorConst.color = { 0.3f, 0.9f, 0.4f, 0.6f }; // Semi-transparent green
    AddBind(PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConst, 0u));

    // Add the locally defined BlendState
    AddBind(std::make_shared<SolidBoxBlendState>(gfx));

    // TransformCbuf remains unique to this instance
    AddBind(std::make_shared<TransformCbuf>(gfx, *this));
}

// --- GetTransformXM returns the stored matrix ---
DirectX::XMMATRIX SolidBox::GetTransformXM() const noexcept
{
    return DirectX::XMLoadFloat4x4(&matrix);
}

// --- SetTransformXM updates the stored matrix ---
void SolidBox::SetTransformXM(DirectX::FXMMATRIX matrixIn) noexcept
{
    DirectX::XMStoreFloat4x4(&matrix, matrixIn);
}
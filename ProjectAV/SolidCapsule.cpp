#include "SolidCapsule.h"
#include "BindableCommon.h"      
#include "GraphicsThrowMacros.h" 
#include "Vertex.h"              
#include "Capsule.h"            // Include the Capsule geometry generator
#include "Bindable.h"            
#include <memory>                
#include <string>                
#include <sstream>               

// Use necessary namespaces
namespace dx = DirectX;
using namespace DirectX::SimpleMath; // For Vector3

namespace Bind {
    class SolidCapsuleBlendState : public Bindable {
        // ... (Blend state implementation remains the same) ...
    public:
        SolidCapsuleBlendState(Graphics& gfx) {
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
}
// --- SolidCapsule Constructor ---
SolidCapsule::SolidCapsule(Graphics& gfx, Vector3 base, Vector3 tip, float radius) :
    basePos(base), // Initialize members
    tipPos(tip),
    capRadius(radius)
{
    using namespace Bind;
    namespace dx = DirectX;

    // --- Generate capsule geometry using WORLD coordinates ---
    // Convert Vector3 to XMFLOAT3 for Capsule::Make
    DirectX::XMFLOAT3 baseF3 = { base.x, base.y, base.z };
    DirectX::XMFLOAT3 tipF3 = { tip.x, tip.y, tip.z };
    auto model = Capsule::Make(baseF3, tipF3, radius);
    // --- End geometry generation ---

    // --- Create unique geometry tag based on parameters ---
    // This won't really cache well with world coordinates, but follows pattern
    std::stringstream ss;
    ss << std::fixed << "$capsule_" << radius << "_"
        << base.x << "_" << base.y << "_" << base.z << "_"
        << tip.x << "_" << tip.y << "_" << tip.z;
    const auto geometryTag = ss.str();

    // Resolve shared bindables via Codex (Geometry won't be shared well here)
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
    colorConst.color = { 0.9f, 0.4f, 0.3f, 0.6f }; // Semi-transparent orange
    AddBind(PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConst, 0u));

    // Add the locally defined BlendState
    AddBind(std::make_shared<SolidCapsuleBlendState>(gfx)); // Requires definition

    // TransformCbuf - uses GetTransformXM which returns Identity for this approach
    AddBind(std::make_shared<TransformCbuf>(gfx, *this));
}

// --- Setters update stored members ---
void SolidCapsule::SetBase(Vector3 base) noexcept
{
    this->basePos = base;
    // PROBLEM: Doesn't update geometry if using temporary approach
}
void SolidCapsule::SetTip(Vector3 tip) noexcept
{
    this->tipPos = tip;
    // PROBLEM: Doesn't update geometry if using temporary approach
}
void SolidCapsule::SetRadius(float radius) noexcept
{
    this->capRadius = radius;
    // PROBLEM: Doesn't update geometry if using temporary approach
}

// --- GetTransformXM for temporary approach ---
DirectX::XMMATRIX SolidCapsule::GetTransformXM() const noexcept
{
    // Since the geometry is created in world space each frame,
    // the transform applied by the shader should be identity.
    return DirectX::XMMatrixIdentity();
}
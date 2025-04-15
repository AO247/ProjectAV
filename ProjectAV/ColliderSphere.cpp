// ColliderSphere.cpp
#include "ColliderSphere.h"
#include "BindableCommon.h" // We still need other common bindables
#include "GraphicsThrowMacros.h"
#include "Vertex.h"
#include "Sphere.h"
#include "Bindable.h"     // Include the base Bindable class definition
#include <memory>         // For std::make_shared

// --- Local BlendState Definition (Inside ColliderSphere.cpp) ---
namespace Bind
{
    // Define a simple BlendState class locally just for ColliderSphere's use.
    // It inherits from Bindable but isn't meant for the Codex.
    class ColliderSphereBlendState : public Bindable
    {
    public:
        ColliderSphereBlendState(Graphics& gfx)
        {
            INFOMAN(gfx);
            D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC{ CD3D11_DEFAULT{} };
            auto& brt = blendDesc.RenderTarget[0];

            // Setup standard alpha blending
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

        void Bind(Graphics& gfx) noexcept override
        {
            GetContext(gfx)->OMSetBlendState(pBlender.Get(), nullptr, 0xFFFFFFFFu);
        }

    protected:
        Microsoft::WRL::ComPtr<ID3D11BlendState> pBlender;
    };
} // namespace Bind
// --- End Local BlendState Definition ---


// --- ColliderSphere Constructor ---
ColliderSphere::ColliderSphere(Graphics& gfx, float radius)
{
    using namespace Bind; // Still use Bind namespace for other bindables
    namespace dx = DirectX;

    auto model = Sphere::Make();
    model.Transform(dx::XMMatrixScaling(radius, radius, radius));
    const auto geometryTag = "$sphere." + std::to_string(radius);

    // Resolve shared bindables via Codex
    AddBind(VertexBuffer::Resolve(gfx, geometryTag, model.vertices));
    AddBind(IndexBuffer::Resolve(gfx, geometryTag, model.indices));
    AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

    auto pvs = VertexShader::Resolve(gfx, "SolidVS.cso");
    auto pvsbc = pvs->GetBytecode();
    AddBind(std::move(pvs)); // Use resolved shared_ptr

    AddBind(PixelShader::Resolve(gfx, "SolidPS.cso")); // Assumes updated shader
    AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

    // Update Constant Buffer for Color+Alpha
    struct PSColorConstant
    {
        dx::XMFLOAT4 color; // Use XMFLOAT4
    } colorConst;
    static_assert(sizeof(PSColorConstant) % 16 == 0, "PSColorConstant size must be multiple of 16!");
    colorConst.color = { 0.3f, 0.4f, 0.9f, 0.6f }; // RGBA - Set alpha here
    AddBind(PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConst, 0u));

    // --- Add the LOCALLY DEFINED BlendState ---
    // Create a unique instance for this sphere using make_shared
    AddBind(std::make_shared<ColliderSphereBlendState>(gfx));
    // --- End BlendState ---

    // TransformCbuf remains unique to this instance
    AddBind(std::make_shared<TransformCbuf>(gfx, *this));
}

// --- Rest of ColliderSphere methods remain the same ---
void ColliderSphere::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
    this->pos = pos;
}

DirectX::XMMATRIX ColliderSphere::GetTransformXM() const noexcept
{
    return DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}
#include "SolidBox.h"
#include "BindableCommon.h"      // Assuming this includes common bindable headers/helpers
#include "GraphicsThrowMacros.h" // For error handling macros
#include "Vertex.h"              // For Dvtx::* types
#include "Box.h"                // Include the AABB mesh generator
#include "Bindable.h"            // Include the base Bindable class definition
#include <memory>                // For std::make_shared
#include <string>                // For std::to_string, included via AABB.h now
#include <sstream>               // For VecToString, included via AABB.h now

namespace { // Anonymous namespace for internal linkage
    inline std::string VecToString(const DirectX::XMFLOAT3& v) {
        std::ostringstream oss;
        // Use fixed format for consistency, especially with floating point numbers
        oss << std::fixed << v.x << "_" << v.y << "_" << v.z;
        return oss.str();
    }
}

// --- Local BlendState Definition (Inside SolidBox.cpp) ---
// Define a simple BlendState class locally just for SolidBox's use.
namespace Bind
{
    class SolidBoxBlendState : public Bindable
    {
    public:
        SolidBoxBlendState(Graphics& gfx)
        {
            INFOMAN(gfx);
            D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC{ CD3D11_DEFAULT{} };
            auto& brt = blendDesc.RenderTarget[0];

            // Setup standard alpha blending
            brt.BlendEnable = TRUE;
            brt.SrcBlend = D3D11_BLEND_SRC_ALPHA;
            brt.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            brt.BlendOp = D3D11_BLEND_OP_ADD;
            brt.SrcBlendAlpha = D3D11_BLEND_ONE;        // Typically not used for standard alpha blend
            brt.DestBlendAlpha = D3D11_BLEND_ZERO;      // Typically not used for standard alpha blend
            brt.BlendOpAlpha = D3D11_BLEND_OP_ADD;      // Typically not used for standard alpha blend
            brt.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

            GFX_THROW_INFO(GetDevice(gfx)->CreateBlendState(&blendDesc, &pBlender));
        }

        void Bind(Graphics& gfx) noexcept override
        {
            // Bind the blend state
            // The blend factor array and sample mask are often nullptr and 0xFFFFFFFFu respectively
            // unless more advanced blending is needed.
            GetContext(gfx)->OMSetBlendState(pBlender.Get(), nullptr, 0xFFFFFFFFu);
        }

        // Static function to create and return a shared_ptr (optional, alternative to make_shared in constructor)
        // static std::shared_ptr<SolidBoxBlendState> Resolve(Graphics& gfx) {
        //    return std::make_shared<SolidBoxBlendState>(gfx);
        // }

    protected:
        Microsoft::WRL::ComPtr<ID3D11BlendState> pBlender;
    };
} // namespace Bind
// --- End Local BlendState Definition ---


// --- SolidBox Constructor ---
SolidBox::SolidBox(Graphics& gfx, DirectX::XMFLOAT3 minExtents, DirectX::XMFLOAT3 maxExtents)
{
    Initialize(gfx, minExtents, maxExtents);
}

// --- SolidBox Initialize ---
void SolidBox::Initialize(Graphics& gfx, DirectX::XMFLOAT3 minExtents, DirectX::XMFLOAT3 maxExtents)
{
    using namespace Bind; // Use Bind namespace for common bindables
    namespace dx = DirectX;

    // Generate the AABB mesh data using the default layout (Position3D)
    auto model = Box::Make(minExtents, maxExtents);
    // Note: No scaling transform needed here.

    // Create a unique tag for geometry caching based on extents
    const auto geometryTag = "$box." + VecToString(minExtents) + "." + VecToString(maxExtents);

    // Resolve shared bindables via Codex
    AddBind(VertexBuffer::Resolve(gfx, geometryTag, model.vertices));
    AddBind(IndexBuffer::Resolve(gfx, geometryTag, model.indices));
    AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

    // Resolve shared shaders and input layout via Codex
    auto pvs = VertexShader::Resolve(gfx, "SolidVS.cso"); // Assuming same VS works
    auto pvsbc = pvs->GetBytecode();
    AddBind(std::move(pvs));

    AddBind(PixelShader::Resolve(gfx, "SolidPS.cso")); // Assuming same PS works BUT expects XMFLOAT4 constant
    AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

    // Update Constant Buffer for Color+Alpha
    struct PSColorConstant
    {
        dx::XMFLOAT4 color; // Use XMFLOAT4 to include alpha
    } colorConst;
    // Ensure the constant buffer size is a multiple of 16 bytes
    static_assert((sizeof(PSColorConstant) % 16) == 0, "PSColorConstant size must be multiple of 16 bytes");
    // Set RGBA color for the collider (e.g., semi-transparent green)
    colorConst.color = { 0.3f, 0.9f, 0.4f, 0.6f }; // Set alpha here (0.6 = 60% opaque)
    // Resolve the pixel constant buffer (assuming slot 0)
    AddBind(PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConst, 0u)); // Use slot 0 like in ColliderSphere

    // --- Add the LOCALLY DEFINED BlendState ---
    // Each SolidBox gets its own BlendState instance, not shared via Codex.
    AddBind(std::make_shared<SolidBoxBlendState>(gfx));
    // --- End BlendState ---

    // TransformCbuf remains unique to this instance
    AddBind(std::make_shared<TransformCbuf>(gfx, *this));
}


// --- Rest of SolidBox methods remain the same ---
void SolidBox::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
    this->pos = pos;
}

DirectX::XMMATRIX SolidBox::GetTransformXM() const noexcept
{
    // Only apply translation for now
    return DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}
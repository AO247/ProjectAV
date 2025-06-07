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

SolidCapsule::SolidCapsule(Graphics& gfx, Vector3 base, Vector3 tip, float radius) :
    basePos(base), // Initialize members
    tipPos(tip),
    capRadius(radius)
{
    using namespace Bind;
    namespace dx = DirectX;

    // --- Generate capsule geometry using WORLD coordinates ---
    // Convert Vector3 to XMFLOAT3 for Capsule::Make
    DirectX::XMFLOAT3 baseF3 = { basePos.x, basePos.y, basePos.z };
    DirectX::XMFLOAT3 tipF3 = { tipPos.x, tipPos.y, tipPos.z };
    auto model = Capsule::Make(baseF3, tipF3, capRadius);
    // --- End geometry generation ---

    // --- Create unique geometry tag based on parameters ---
    // This won't really cache well with world coordinates, but follows pattern
    std::stringstream ss;
    ss << std::fixed << "$capsule_" << capRadius << "_"
        << basePos.x << "_" << basePos.y << "_" << basePos.z << "_"
        << tipPos.x << "_" << tipPos.y << "_" << tipPos.z;
    const auto geometryTag = ss.str();

    pVertices = VertexBuffer::Resolve(gfx, geometryTag, model.vertices);
    pIndices = IndexBuffer::Resolve(gfx, geometryTag, model.indices);
    pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    {
        Technique colliderTechnique("ColliderSolid");
        Step mainPass(0);

        // --- Vertex Shader ---
        auto pvs = VertexShader::Resolve(gfx, "Solid_VS.cso");
        auto pvsbc = pvs->GetBytecode();
        mainPass.AddBindable(std::move(pvs));

        // --- Pixel Shader ---
        mainPass.AddBindable(PixelShader::Resolve(gfx, "Solid_PS.cso"));

        // --- Pixel Constant Buffer for Color ---
        struct PSColorConstant { dx::XMFLOAT4 color; } colorConst;
        static_assert(sizeof(PSColorConstant) % 16 == 0);
        colorConst.color = { 0.7f, 0.0f, 0.0f, 0.6f };
        mainPass.AddBindable(std::make_shared<PixelConstantBuffer<PSColorConstant>>(gfx, colorConst, 1u));

        // --- Input Layout ---
        //mainPass.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

        // --- Transform Constant Buffer ---
        mainPass.AddBindable(std::make_shared<TransformCbuf>(gfx)); // Assuming default slot 0 for VS

        // --- Rasterizer State ---
        mainPass.AddBindable(Rasterizer::Resolve(gfx, false)); // false for default (cull back)

        // --- **** CORRECTED ORDER FOR BLENDING **** ---
        // 1. Bind Stencil first (if it modifies blend state, its effect is set)
        mainPass.AddBindable(Stencil::Resolve(gfx, Stencil::Mode::Off));

        // 2. THEN Bind your Blender to *override* with desired blend settings
        //    Pass 'true' to enable blending, and std::nullopt for factor to use SrcAlpha/InvSrcAlpha
        mainPass.AddBindable(Blender::Resolve(gfx, true));
        // --- **** END CORRECTION **** ---

        colliderTechnique.AddStep(std::move(mainPass));
        AddTechnique(std::move(colliderTechnique));
    }
}

void SolidCapsule::Update(Graphics& gfx) {
    using namespace Bind;
    namespace dx = DirectX;

    // --- Generate capsule geometry using WORLD coordinates ---
    // Convert Vector3 to XMFLOAT3 for Capsule::Make
    DirectX::XMFLOAT3 baseF3 = { basePos.x, basePos.y, basePos.z };
    DirectX::XMFLOAT3 tipF3 = { tipPos.x, tipPos.y, tipPos.z };
    auto model = Capsule::Make(baseF3, tipF3, capRadius);
    // --- End geometry generation ---

    // --- Create unique geometry tag based on parameters ---
    // This won't really cache well with world coordinates, but follows pattern
    std::stringstream ss;
    ss << std::fixed << "$capsule_" << capRadius << "_"
        << basePos.x << "_" << basePos.y << "_" << basePos.z << "_"
        << tipPos.x << "_" << tipPos.y << "_" << tipPos.z;
    const auto geometryTag = ss.str();

    pVertices = VertexBuffer::Resolve(gfx, geometryTag, model.vertices);
    pIndices = IndexBuffer::Resolve(gfx, geometryTag, model.indices);
    pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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
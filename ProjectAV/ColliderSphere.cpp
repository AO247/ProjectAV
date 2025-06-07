#include "ColliderSphere.h"
#include "BindableCommon.h"      // Includes VertexBuffer, IndexBuffer, Shaders, etc.
#include "GraphicsThrowMacros.h" // For INFOMAN
#include "Vertex.h"              // For Dvtx::* types
#include "Sphere.h"              // For Sphere::Make()
#include "Stencil.h"             // Include if using Stencil (example SolidSphere might not, but good practice)
#include "ConstantBuffers.h"     // For PixelConstantBuffer<T>
#include "Channels.h"

// No need for a local ColliderSphereBlendState if your BindableCommon.h
// already includes a general Blender::Resolve(gfx, bool blendEnabled)
// If not, you'd keep the local BlendState definition from your original code.
// For this adaptation, I'll assume Blender::Resolve(gfx, true) enables alpha blending.

namespace dx = DirectX;

ColliderSphere::ColliderSphere(Graphics& gfx, float radius)
{
    using namespace Bind;
    namespace dx = DirectX;

    // ... (Geometry setup: model, geometryTag) ...
    auto model = Sphere::Make();
    model.Transform(dx::XMMatrixScaling(radius, radius, radius));
    const auto geometryTag = "$debug_sphere." + std::to_string(radius);

    pVertices = VertexBuffer::Resolve(gfx, geometryTag, model.vertices);
    pIndices = IndexBuffer::Resolve(gfx, geometryTag, model.indices);
    pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    {
        Technique colliderTechnique("ColliderSolid", Chan::main);
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
        colorConst.color = { 0.0f, 0.0f, 0.7f, 0.6f };
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

    pos = { 0.0f, 0.0f, 0.0f };
}

void ColliderSphere::SetPos(DirectX::XMFLOAT3 newPos) noexcept
{
    this->pos = newPos;
}

DirectX::XMMATRIX ColliderSphere::GetTransformXM() const noexcept
{
    // The sphere geometry is a unit sphere scaled by radius, centered at origin.
    // This transform matrix will place it in the world.
    return DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}
#include "ColliderSphere.h"
#include "BindableCommon.h"      
#include "GraphicsThrowMacros.h" 
#include "Vertex.h"              
#include "Sphere.h"              
#include "Stencil.h"             
#include "ConstantBuffers.h"     


namespace dx = DirectX;

ColliderSphere::ColliderSphere(Graphics& gfx, float radius)
{
    using namespace Bind;
    namespace dx = DirectX;

    auto model = Sphere::Make();
    model.Transform(dx::XMMatrixScaling(radius, radius, radius));
    const auto geometryTag = "$debug_sphere." + std::to_string(radius);

    pVertices = VertexBuffer::Resolve(gfx, geometryTag, model.vertices);
    pIndices = IndexBuffer::Resolve(gfx, geometryTag, model.indices);
    pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    {
        Technique colliderTechnique("ColliderSolid");
        Step mainPass(0);

        auto pvs = VertexShader::Resolve(gfx, "Solid_VS.cso");
        auto pvsbc = pvs->GetBytecode();
        mainPass.AddBindable(std::move(pvs));

        mainPass.AddBindable(PixelShader::Resolve(gfx, "Solid_PS.cso"));

        struct PSColorConstant { dx::XMFLOAT4 color; } colorConst;
        static_assert(sizeof(PSColorConstant) % 16 == 0);
        colorConst.color = { 0.0f, 0.0f, 0.7f, 0.6f };
        mainPass.AddBindable(std::make_shared<PixelConstantBuffer<PSColorConstant>>(gfx, colorConst, 1u));


        mainPass.AddBindable(std::make_shared<TransformCbuf>(gfx));

        mainPass.AddBindable(Rasterizer::Resolve(gfx, false));

        mainPass.AddBindable(Stencil::Resolve(gfx, Stencil::Mode::Off));

        mainPass.AddBindable(Blender::Resolve(gfx, true));

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
    return DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}
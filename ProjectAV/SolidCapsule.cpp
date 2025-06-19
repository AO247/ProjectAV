#include "SolidCapsule.h"
#include "BindableCommon.h"      
#include "GraphicsThrowMacros.h" 
#include "Vertex.h"              
#include "Capsule.h"           
#include "Bindable.h"            
#include <memory>                
#include <string>                
#include <sstream>               

namespace dx = DirectX;
using namespace DirectX::SimpleMath; 

SolidCapsule::SolidCapsule(Graphics& gfx, Vector3 base, Vector3 tip, float radius) :
    basePos(base),
    tipPos(tip),
    capRadius(radius)
{
    using namespace Bind;
    namespace dx = DirectX;

    DirectX::XMFLOAT3 baseF3 = { basePos.x, basePos.y, basePos.z };
    DirectX::XMFLOAT3 tipF3 = { tipPos.x, tipPos.y, tipPos.z };
    auto model = Capsule::Make(baseF3, tipF3, capRadius);

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


        auto pvs = VertexShader::Resolve(gfx, "Solid_VS.cso");
        auto pvsbc = pvs->GetBytecode();
        mainPass.AddBindable(std::move(pvs));

        mainPass.AddBindable(PixelShader::Resolve(gfx, "Solid_PS.cso"));


        struct PSColorConstant { dx::XMFLOAT4 color; } colorConst;
        static_assert(sizeof(PSColorConstant) % 16 == 0);
        colorConst.color = { 0.7f, 0.0f, 0.0f, 0.6f };
        mainPass.AddBindable(std::make_shared<PixelConstantBuffer<PSColorConstant>>(gfx, colorConst, 1u));


        mainPass.AddBindable(std::make_shared<TransformCbuf>(gfx)); 


        mainPass.AddBindable(Rasterizer::Resolve(gfx, false));


        mainPass.AddBindable(Stencil::Resolve(gfx, Stencil::Mode::Off));

        mainPass.AddBindable(Blender::Resolve(gfx, true));


        colliderTechnique.AddStep(std::move(mainPass));
        AddTechnique(std::move(colliderTechnique));
    }
}

void SolidCapsule::Update(Graphics& gfx) {
    using namespace Bind;
    namespace dx = DirectX;


    DirectX::XMFLOAT3 baseF3 = { basePos.x, basePos.y, basePos.z };
    DirectX::XMFLOAT3 tipF3 = { tipPos.x, tipPos.y, tipPos.z };
    auto model = Capsule::Make(baseF3, tipF3, capRadius);


    std::stringstream ss;
    ss << std::fixed << "$capsule_" << capRadius << "_"
        << basePos.x << "_" << basePos.y << "_" << basePos.z << "_"
        << tipPos.x << "_" << tipPos.y << "_" << tipPos.z;
    const auto geometryTag = ss.str();

    pVertices = VertexBuffer::Resolve(gfx, geometryTag, model.vertices);
    pIndices = IndexBuffer::Resolve(gfx, geometryTag, model.indices);
    pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void SolidCapsule::SetBase(Vector3 base) noexcept
{
    this->basePos = base;

}
void SolidCapsule::SetTip(Vector3 tip) noexcept
{
    this->tipPos = tip;

}
void SolidCapsule::SetRadius(float radius) noexcept
{
    this->capRadius = radius;

}


DirectX::XMMATRIX SolidCapsule::GetTransformXM() const noexcept
{

    return DirectX::XMMatrixIdentity();
}
#include "SolidBox.h"
#include "BindableCommon.h"      
#include "GraphicsThrowMacros.h" 
#include "Vertex.h"              
#include "Box.h"                
#include "Bindable.h"            
#include <memory>                
#include <string>                
#include <sstream>               

namespace { 

    inline std::string VecToString(const DirectX::XMFLOAT3& v) {
        std::ostringstream oss;
        oss << std::fixed << v.x << "_" << v.y << "_" << v.z;
        return oss.str();
    }
}



SolidBox::SolidBox(Graphics& gfx, DirectX::XMFLOAT3 center, DirectX::XMFLOAT3 size)
{
    using namespace Bind;
    namespace dx = DirectX;


    DirectX::XMFLOAT3 halfSize = { size.x / 2.0f, size.y / 2.0f, size.z / 2.0f };

    DirectX::XMFLOAT3 minExtents = { -halfSize.x, -halfSize.y, -halfSize.z };
    DirectX::XMFLOAT3 maxExtents = { halfSize.x,  halfSize.y,  halfSize.z };

    auto model = Box::Make(minExtents, maxExtents);


    const auto geometryTag = "$box." + VecToString(size);

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
        colorConst.color = { 0.0f, 0.7f, 0.0f, 0.6f };
        mainPass.AddBindable(std::make_shared<PixelConstantBuffer<PSColorConstant>>(gfx, colorConst, 1u));



        mainPass.AddBindable(std::make_shared<TransformCbuf>(gfx));

        mainPass.AddBindable(Rasterizer::Resolve(gfx, false));


        mainPass.AddBindable(Stencil::Resolve(gfx, Stencil::Mode::Off));


        mainPass.AddBindable(Blender::Resolve(gfx, true));


        colliderTechnique.AddStep(std::move(mainPass));
        AddTechnique(std::move(colliderTechnique));
    }
}


void SolidBox::SetPos(DirectX::XMFLOAT3 center) noexcept
{
    this->center = center;
}
void SolidBox::SetSize(DirectX::XMFLOAT3 size) noexcept
{
	this->size = size;
}
DirectX::XMMATRIX SolidBox::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation(center.x, center.y, center.z);
}


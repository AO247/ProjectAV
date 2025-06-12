#include "DebugLine.h"
#include "BindableCommon.h"      
#include "GraphicsThrowMacros.h" 
#include "Vertex.h"              
#include "Technique.h"           
#include "ConstantBuffers.h"     
#include "Stencil.h"             
#include <sstream>               

namespace dx = DirectX;

std::string DebugLine::GenerateLineTag(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end) {
    std::ostringstream oss;
    oss << "$debug_line_"
        << start.x << "_" << start.y << "_" << start.z << "_"
        << end.x << "_" << end.y << "_" << end.z;
    return oss.str();
}


DebugLine::DebugLine(Graphics& gfx, dx::XMFLOAT3 start, dx::XMFLOAT3 end, dx::XMFLOAT4 color)
    : startPoint(start), endPoint(end)
{
    using namespace Bind;

    Dvtx::VertexLayout layout = Dvtx::VertexLayout{}.Append(Dvtx::VertexLayout::Position3D);
    Dvtx::VertexBuffer vbuf{ layout };
    vbuf.EmplaceBack(startPoint);
    vbuf.EmplaceBack(endPoint);  

    std::vector<unsigned short> indices = { 0, 1 }; 

    pVertices = std::make_shared<VertexBuffer>(gfx, vbuf);
    pIndices = std::make_shared<IndexBuffer>(gfx, indices);
    pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_LINELIST); 

    {
        Technique lineTechnique("DebugLineSolid"); 
        Step mainPass(0); 

        auto pvs = VertexShader::Resolve(gfx, "Solid_VS.cso");
        auto pvsbc = pvs->GetBytecode();
        mainPass.AddBindable(std::move(pvs));


        mainPass.AddBindable(PixelShader::Resolve(gfx, "Solid_PS.cso"));


        struct PSColorConstant { dx::XMFLOAT4 color; } colorConst;
        static_assert(sizeof(PSColorConstant) % 16 == 0, "PSColorConstant size must be multiple of 16 bytes");
        colorConst.color = color;

        mainPass.AddBindable(std::make_shared<PixelConstantBuffer<PSColorConstant>>(gfx, colorConst, 1u)); 

        mainPass.AddBindable(std::make_shared<TransformCbuf>(gfx, 0u));


        bool lineHasAlpha = color.w < 1.0f;
        mainPass.AddBindable(Blender::Resolve(gfx, lineHasAlpha));


        mainPass.AddBindable(Rasterizer::Resolve(gfx, false)); 

        mainPass.AddBindable(Stencil::Resolve(gfx, Stencil::Mode::Off));

        lineTechnique.AddStep(std::move(mainPass));
        AddTechnique(std::move(lineTechnique));
    }
}


DirectX::XMMATRIX DebugLine::GetTransformXM() const noexcept
{

    return dx::XMMatrixIdentity();
}

void DebugLine::SetPoints(Graphics& gfx, DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end)
{
	startPoint = start;
	endPoint = end;

    using namespace Bind;


    Dvtx::VertexLayout layout = Dvtx::VertexLayout{}.Append(Dvtx::VertexLayout::Position3D);
    Dvtx::VertexBuffer vbuf{ layout };
    vbuf.EmplaceBack(startPoint); 
    vbuf.EmplaceBack(endPoint);   

    std::vector<unsigned short> indices = { 0, 1 }; 

    pVertices = std::make_shared<VertexBuffer>(gfx, vbuf);
    pIndices = std::make_shared<IndexBuffer>(gfx, indices);
    pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_LINELIST); 

}
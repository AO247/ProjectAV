#include "DebugLine.h"
#include "BindableCommon.h"
#include "GraphicsThrowMacros.h"
#include "Vertex.h"
#include <sstream> // For tag generation

namespace dx = DirectX;

// Helper to create unique tag
std::string DebugLine::GenerateLineTag(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end) {
    std::ostringstream oss;
    oss << "$line_"
        << start.x << "_" << start.y << "_" << start.z << "_"
        << end.x << "_" << end.y << "_" << end.z;
    return oss.str();
}


DebugLine::DebugLine(Graphics& gfx, dx::XMFLOAT3 start, dx::XMFLOAT3 end, dx::XMFLOAT4 color)
    : startPoint(start), endPoint(end)
{
    using namespace Bind;

    // --- Geometry ---
    // Layout: Position only for basic line
    Dvtx::VertexLayout layout = Dvtx::VertexLayout{}.Append(Dvtx::VertexLayout::Position3D);
    Dvtx::VertexBuffer vbuf{ layout };
    vbuf.EmplaceBack(startPoint); // Vertex 0: start
    vbuf.EmplaceBack(endPoint);   // Vertex 1: end

    std::vector<unsigned short> indices = { 0, 1 }; // Indices for a single line segment

    // Unique tag based on initial points (though inefficient if points change often)
    // For debug drawing, creating unique non-cached buffers might be simpler.
    // Let's skip Codex for debug lines for now to avoid tag complexity.
    // const auto geometryTag = GenerateLineTag(start, end); 

    // --- Bindables ---
    // Create unique vertex/index buffers directly (not using Resolve)
    AddBind(std::make_shared<VertexBuffer>(gfx, vbuf)); // Pass VB directly
    AddBind(std::make_shared<IndexBuffer>(gfx, indices)); // Pass indices directly

    // Shaders (Use Solid shaders, assuming they take position and color constant)
    auto pvs = VertexShader::Resolve(gfx, "SolidVS.cso");
    auto pvsbc = pvs->GetBytecode();
    AddBind(std::move(pvs));
    AddBind(PixelShader::Resolve(gfx, "SolidPS.cso")); // Assumes PS uses color constant

    // Input Layout
    AddBind(InputLayout::Resolve(gfx, layout, pvsbc));

    // Topology: Use LineList
    AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_LINELIST));

    // Constant Buffer for Color (using unique instance)
    struct PSColorConstant { dx::XMFLOAT4 color; } colorConst;
    static_assert((sizeof(PSColorConstant) % 16) == 0, "PSColorConstant size must be multiple of 16 bytes");
    colorConst.color = color; // Use the passed color
    AddBind(std::make_shared<PixelConstantBuffer<PSColorConstant>>(gfx, colorConst, 0u));

    // Transform Buffer (returns Identity since vertices are world space)
    AddBind(std::make_shared<TransformCbuf>(gfx, *this));
}

// Optional: Update points if reusing the DebugLine object
// This would require updating the Vertex Buffer, which is complex with the current setup.
// It's simpler to create new DebugLine objects each frame for debug visualization.
/*
void DebugLine::SetPoints(Graphics& gfx, DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end)
{
    startPoint = start;
    endPoint = end;
    // !!! Need to update the Vertex Buffer here !!!
    // This requires getting the VertexBuffer bindable and calling Map/Update/Unmap
    // Example (conceptual):
    // auto pVB = QueryBindable<Bind::VertexBuffer>();
    // if(pVB) {
    //    Dvtx::VertexBuffer vbuf(Dvtx::VertexLayout{}.Append(Dvtx::VertexLayout::Position3D));
    //    vbuf.EmplaceBack(startPoint);
    //    vbuf.EmplaceBack(endPoint);
    //    pVB->Update(gfx, vbuf); // Assumes VertexBuffer has an Update method
    // }
}
*/

DirectX::XMMATRIX DebugLine::GetTransformXM() const noexcept
{
    // Vertices are already in world space for this debug line approach
    return dx::XMMatrixIdentity();
}
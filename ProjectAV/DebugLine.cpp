#include "DebugLine.h"
#include "BindableCommon.h"      // For VertexBuffer, IndexBuffer, Topology, Shaders, ConstantBuffers, etc.
#include "GraphicsThrowMacros.h" // For INFOMAN
#include "Vertex.h"              // For Dvtx::* types
#include "Technique.h"           // For Technique and Step
#include "ConstantBuffers.h"     // For PixelConstantBuffer (if not already in BindableCommon)
#include "Stencil.h"             // For Stencil state
#include <sstream>               // For tag generation (though less critical for non-cached debug lines)

namespace dx = DirectX;

// Helper to create unique tag (less important if not using Codex for geometry)
std::string DebugLine::GenerateLineTag(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end) {
    std::ostringstream oss;
    oss << "$debug_line_"
        << start.x << "_" << start.y << "_" << start.z << "_"
        << end.x << "_" << end.y << "_" << end.z;
    return oss.str();
}


DebugLine::DebugLine(Graphics& gfx, dx::XMFLOAT3 start, dx::XMFLOAT3 end, dx::XMFLOAT4 color)
    : startPoint(start), endPoint(end) // Store points if needed for GetTransformXM (not for Identity)
{
    using namespace Bind;

    // --- 1. Prepare Geometry ---
    // Layout: Position only for basic line
    Dvtx::VertexLayout layout = Dvtx::VertexLayout{}.Append(Dvtx::VertexLayout::Position3D);
    Dvtx::VertexBuffer vbuf{ layout };
    vbuf.EmplaceBack(startPoint); // Vertex 0: start point (in world space)
    vbuf.EmplaceBack(endPoint);   // Vertex 1: end point (in world space)

    std::vector<unsigned short> indices = { 0, 1 }; // Indices for a single line segment

    // For debug lines, we usually don't cache geometry via Codex because points change constantly.
    // So, we create unique vertex/index buffers directly.
    pVertices = std::make_shared<VertexBuffer>(gfx, vbuf);
    pIndices = std::make_shared<IndexBuffer>(gfx, indices);
    pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_LINELIST); // Use LineList for lines

    // --- 2. Define Rendering Technique ---
    {
        Technique lineTechnique("DebugLineSolid"); // Name the technique
        Step mainPass(0); // Step targets rendering pass 0 (or a dedicated debug pass if you have one)

        // --- Vertex Shader ---
        // Assuming Solid_VS.cso can handle simple position transformation
        auto pvs = VertexShader::Resolve(gfx, "Solid_VS.cso");
        auto pvsbc = pvs->GetBytecode();
        mainPass.AddBindable(std::move(pvs));

        // --- Pixel Shader ---
        // Assuming Solid_PS.cso can use a color from a constant buffer
        mainPass.AddBindable(PixelShader::Resolve(gfx, "Solid_PS.cso"));

        // --- Pixel Constant Buffer for Line Color ---
        struct PSColorConstant { dx::XMFLOAT4 color; } colorConst;
        static_assert(sizeof(PSColorConstant) % 16 == 0, "PSColorConstant size must be multiple of 16 bytes");
        colorConst.color = color; // Use the passed-in color
        // Create a unique constant buffer for this line instance, not shared
        mainPass.AddBindable(std::make_shared<PixelConstantBuffer<PSColorConstant>>(gfx, colorConst, 1u)); // Slot 1 for material/object color

        // --- Input Layout ---
        //mainPass.AddBindable(InputLayout::Resolve(gfx, layout, *pvs));

        // --- Transform Constant Buffer ---
        // Since vertices are already in world space, GetTransformXM() returns Identity.
        // This TransformCbuf will apply ViewProjection to the world-space vertices.
        mainPass.AddBindable(std::make_shared<TransformCbuf>(gfx, 0u)); // Slot 0 for VS transform

        // --- Blend State (Optional, lines are usually opaque) ---
        // If you want transparent lines, enable blending with hasAlphaDiffuse=true for your line color
        bool lineHasAlpha = color.w < 1.0f;
        mainPass.AddBindable(Blender::Resolve(gfx, lineHasAlpha));

        // --- Rasterizer State (Optional, default usually works) ---
        // For lines, culling is often irrelevant, but default is fine.
        // Wireframe mode doesn't make sense for a line list.
        mainPass.AddBindable(Rasterizer::Resolve(gfx, false)); // false for default (cull back)

        // --- Stencil State (Usually off for debug lines) ---
        mainPass.AddBindable(Stencil::Resolve(gfx, Stencil::Mode::Off));

        lineTechnique.AddStep(std::move(mainPass));
        AddTechnique(std::move(lineTechnique)); // Add the technique to this drawable
    }
}


DirectX::XMMATRIX DebugLine::GetTransformXM() const noexcept
{
    // The vertices for DebugLine are already defined in world space.
    // Therefore, the "model" transform is identity. The TransformCbuf will still
    // apply the view-projection matrix to these world-space vertices in the shader.
    return dx::XMMatrixIdentity();
}

void DebugLine::SetPoints(Graphics& gfx, DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end)
{
	startPoint = start;
	endPoint = end;

    using namespace Bind;

    // --- 1. Prepare Geometry ---
    // Layout: Position only for basic line
    Dvtx::VertexLayout layout = Dvtx::VertexLayout{}.Append(Dvtx::VertexLayout::Position3D);
    Dvtx::VertexBuffer vbuf{ layout };
    vbuf.EmplaceBack(startPoint); // Vertex 0: start point (in world space)
    vbuf.EmplaceBack(endPoint);   // Vertex 1: end point (in world space)

    std::vector<unsigned short> indices = { 0, 1 }; // Indices for a single line segment

    // For debug lines, we usually don't cache geometry via Codex because points change constantly.
    // So, we create unique vertex/index buffers directly.
    pVertices = std::make_shared<VertexBuffer>(gfx, vbuf);
    pIndices = std::make_shared<IndexBuffer>(gfx, indices);
    pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_LINELIST); // Use LineList for lines

}
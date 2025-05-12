#include "DebugLine.h"
#include "BindableCommon.h"      // For VertexBuffer, IndexBuffer, Shaders, etc.
#include "GraphicsThrowMacros.h" // For INFOMAN
#include "Vertex.h"              // For Dvtx::* types
#include "Technique.h"           // For Technique and Step
#include "ConstantBuffers.h"     // For PixelConstantBuffer
#include "Stencil.h"             // If you standardize and add Stencil
#include "Blender.h"             // If you standardize and add Blender

#include <sstream> // For tag generation (though we might not cache debug line geometry)

namespace dx = DirectX;

// Helper to create unique tag (less useful if not caching via Resolve)
std::string DebugLine::GenerateLineTag(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end) {
    std::ostringstream oss;
    oss << "$debug_line_"
        << start.x << "_" << start.y << "_" << start.z << "_"
        << end.x << "_" << end.y << "_" << end.z;
    return oss.str();
}


DebugLine::DebugLine(Graphics& gfx, dx::XMFLOAT3 start, dx::XMFLOAT3 end, dx::XMFLOAT4 color)
    : startPoint(start), endPoint(end) // Store for potential later use if SetPoints is implemented
{
    using namespace Bind;

    // --- 1. Prepare Geometry ---
    Dvtx::VertexLayout layout = Dvtx::VertexLayout{}.Append(Dvtx::VertexLayout::Position3D);
    Dvtx::VertexBuffer vbuf{ layout };
    vbuf.EmplaceBack(startPoint); // Vertex 0: start point in world space
    vbuf.EmplaceBack(endPoint);   // Vertex 1: end point in world space

    std::vector<unsigned short> indices = { 0, 1 };

    // For debug lines, we typically don't cache their geometry via Resolve
    // because the start/end points change frequently. So, we create unique buffers.
    pVertices = std::make_shared<VertexBuffer>(gfx, vbuf); // Create unique VB
    pIndices = std::make_shared<IndexBuffer>(gfx, indices); // Create unique IB
    pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_LINELIST); // LineList topology is shareable

    // --- 2. Define Rendering Technique ---
    {
        Technique lineTechnique("DebugLineSolid");
        Step mainPass(0); // Assuming debug lines are drawn in the main pass (pass 0)

        // --- Vertex Shader ---
        auto pvs = VertexShader::Resolve(gfx, "Solid_VS.cso"); // Simple VS
        auto pvsbc = pvs->GetBytecode();
        mainPass.AddBindable(std::move(pvs));

        // --- Pixel Shader ---
        mainPass.AddBindable(PixelShader::Resolve(gfx, "Solid_PS.cso")); // Simple PS

        // --- Pixel Constant Buffer for Color ---
        struct PSColorConstant { dx::XMFLOAT4 color; } colorConst;
        static_assert((sizeof(PSColorConstant) % 16) == 0, "PSColorConstant size must be multiple of 16 bytes");
        colorConst.color = color; // Use the passed color
        mainPass.AddBindable(std::make_shared<PixelConstantBuffer<PSColorConstant>>(gfx, colorConst, 1u)); // Slot 1

        // --- Input Layout ---
        mainPass.AddBindable(InputLayout::Resolve(gfx, layout, pvsbc));

        // --- Transform Constant Buffer (unique to this drawable instance) ---
        // GetTransformXM() will return Identity for DebugLine as vertices are world space.
        mainPass.AddBindable(std::make_shared<TransformCbuf>(gfx, 0u));

        // --- Blend State (Debug lines are usually opaque) ---
        mainPass.AddBindable(Blender::Resolve(gfx, false, std::nullopt)); // false = no blending

        // --- Rasterizer State (Default is fine, or no culling for lines) ---
        mainPass.AddBindable(Rasterizer::Resolve(gfx, false)); // false = default (cull back)

        // --- Stencil State (Usually off) ---
        mainPass.AddBindable(Stencil::Resolve(gfx, Stencil::Mode::Off));

        lineTechnique.AddStep(std::move(mainPass));
        AddTechnique(std::move(lineTechnique)); // Add the technique to this drawable
    }
}

// Optional: Update points if reusing the DebugLine object
// This is more complex as it requires updating the GPU vertex buffer.
// For debug lines created per-frame, this isn't typically used.
void DebugLine::SetPoints(Graphics& gfx, DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end)
{
    startPoint = start;
    endPoint = end;

    // If you implement this, you need to:
    // 1. Get the VertexBuffer bindable (e.g., using QueryBindable or storing a direct pointer)
    // 2. Create a new Dvtx::VertexBuffer with the new points
    // 3. Call a method on the GPU VertexBuffer bindable to update its contents (e.g., Map/Unmap)
    // For example:
    // if(pVertices) { // pVertices is std::shared_ptr<Bind::VertexBuffer>
    //    Dvtx::VertexBuffer new_vbuf(Dvtx::VertexLayout{}.Append(Dvtx::VertexLayout::Position3D));
    //    new_vbuf.EmplaceBack(startPoint);
    //    new_vbuf.EmplaceBack(endPoint);
    //    // VertexBuffer class would need an UpdateData(Graphics&, const Dvtx::VertexBuffer&) method
    //    // This is non-trivial to add to the existing VertexBuffer bindable.
    //    // For now, creating new DebugLine instances is simpler.
    // }
    OutputDebugStringA("DebugLine::SetPoints - Vertex buffer update not fully implemented for this example.\n");
}


DirectX::XMMATRIX DebugLine::GetTransformXM() const noexcept
{
    // Since the startPoint and endPoint used to create the vertex buffer
    // are already in world space, the model transform for this line is Identity.
    // The TransformCbuf will still apply View * Projection.
    return dx::XMMatrixIdentity();
}
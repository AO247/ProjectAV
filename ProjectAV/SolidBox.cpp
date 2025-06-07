#include "SolidBox.h"
#include "BindableCommon.h"      
#include "GraphicsThrowMacros.h" 
#include "Vertex.h"              
#include "Box.h"                // Include the Box mesh generator
#include "Bindable.h"            
#include <memory>                
#include <string>                
#include <sstream>        
#include "Channels.h"

namespace { // Anonymous namespace for internal linkage
    // Keep VecToString for tag generation (using size now)
    inline std::string VecToString(const DirectX::XMFLOAT3& v) {
        std::ostringstream oss;
        oss << std::fixed << v.x << "_" << v.y << "_" << v.z;
        return oss.str();
    }
}


// --- **** UPDATED: SolidBox Constructor uses Initialize **** ---
SolidBox::SolidBox(Graphics& gfx, DirectX::XMFLOAT3 center, DirectX::XMFLOAT3 size)
{
    using namespace Bind;
    namespace dx = DirectX;

    // --- Calculate min/max extents LOCALLY for Box::Make ---
    DirectX::XMFLOAT3 halfSize = { size.x / 2.0f, size.y / 2.0f, size.z / 2.0f };
    // Create geometry centered around LOCAL origin (0,0,0) with the given size
    DirectX::XMFLOAT3 minExtents = { -halfSize.x, -halfSize.y, -halfSize.z };
    DirectX::XMFLOAT3 maxExtents = { halfSize.x,  halfSize.y,  halfSize.z };
    // --- End Calculation ---

    // Generate box mesh data centered at origin with the specified size
    auto model = Box::Make(minExtents, maxExtents);

    // Create geometry tag based ONLY on SIZE, as center offset is handled by transform
    const auto geometryTag = "$box." + VecToString(size);

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
        colorConst.color = { 0.0f, 0.7f, 0.0f, 0.6f };
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


void SolidBox::SetPos(DirectX::XMFLOAT3 center) noexcept
{
    this->center = center;
}
void SolidBox::SetSize(DirectX::XMFLOAT3 size) noexcept
{
	this->size = size;
}
// --- GetTransformXM returns the stored matrix ---
DirectX::XMMATRIX SolidBox::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation(center.x, center.y, center.z);
}


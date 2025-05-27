#include "FrameCommander.h"
#include "BindableCodex.h"
#include "GraphicsThrowMacros.h"
#include "GraphicsResource.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "Sampler.h"
#include "Blender.h"
#include "ConstantBuffers.h"
#include "RenderTarget.h"     // Included
#include "DepthStencil.h"       // Included
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "InputLayout.h"
#include "imgui/imgui.h"
#include <d3d11.h> // For D3D11_DEPTH_STENCIL_DESC etc.

namespace dx = DirectX;
using namespace Bind;

// In FrameCommander.cpp
#include "GraphicsResource.h" // Ensure this is included

Microsoft::WRL::ComPtr<ID3D11DepthStencilState> CreateDepthStencilState(
    Graphics& gfx, const D3D11_DEPTH_STENCIL_DESC& desc)
{
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pState;
    HRESULT hr; // The macros expect 'hr' to be in scope.
    ID3D11Device* pDevice = GraphicsResource::GetDevice(gfx); // Correctly scoped call

#ifndef NDEBUG
    // Manually define 'infoManager' for the GFX_THROW_INFO macro to use.
    DxgiInfoManager& infoManager = GraphicsResource::GetInfoManager(gfx); // Correctly scoped call

    // GFX_THROW_INFO will use the 'hr' and 'infoManager' variables we just defined.
    GFX_THROW_INFO(pDevice->CreateDepthStencilState(&desc, &pState));
#else
    // In Release, GFX_THROW_INFO expands to GFX_THROW_NOINFO, which only uses 'hr'.
    // We don't need to (and shouldn't) define 'infoManager'.
    GFX_THROW_INFO(pDevice->CreateDepthStencilState(&desc, &pState));
#endif
    return pState;
}
// Helper function to create a Null Pixel Shader (if you don't have a Bindable for it)
Microsoft::WRL::ComPtr<ID3D11PixelShader> CreateNullPixelShader(Graphics& gfx) {
    // A shader that does nothing or discards.
    // const char* shaderCode = "void main() { discard; }"; // Simplest
    // Or: "float4 main() : SV_TARGET { return float4(0,0,0,0); }" with RenderTargetWriteMask = 0 on blend state
    // For simplicity, let's assume a precompiled "Null_PS.cso" that just returns/discards.
    // This part is tricky without knowing your shader compilation pipeline.
    // If passes[PassOutlineMask] jobs provide their own "null" pixel shader, this isn't needed here.
    // For now, this will be a placeholder.
    // You should use your PixelShader::Resolve(gfx, "Null_PS.cso") if you have such a shader.
    return nullptr; // Placeholder - jobs in PassOutlineMask should handle their PS.
}


FrameCommander::FrameCommander(Graphics& gfx)
    :
    ds(gfx, gfx.GetWidth(), gfx.GetHeight()),
    rt1({ gfx, gfx.GetWidth() / downFactor, gfx.GetHeight() / downFactor }), // downFactor is for outline blur
    rt2({ gfx, gfx.GetWidth() / downFactor, gfx.GetHeight() / downFactor }),
    blur(gfx, 7, 2.6f, "BlurOutline_PS.cso")
{
    // --- Initialize NEW Render Targets for Motion Blur ---
    sceneColorRT.emplace(gfx, gfx.GetWidth(), gfx.GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
    velocityRT.emplace(gfx, gfx.GetWidth(), gfx.GetHeight(), DXGI_FORMAT_R16G16_FLOAT);

    // --- Fullscreen Quad ---
    Dvtx::VertexLayout lay;
    lay.Append(Dvtx::VertexLayout::Position2D);
    Dvtx::VertexBuffer vbuf_full{ lay };
    vbuf_full.EmplaceBack(dx::XMFLOAT2{ -1.0f, 1.0f });
    vbuf_full.EmplaceBack(dx::XMFLOAT2{ 1.0f, 1.0f });
    vbuf_full.EmplaceBack(dx::XMFLOAT2{ -1.0f, -1.0f });
    vbuf_full.EmplaceBack(dx::XMFLOAT2{ 1.0f, -1.0f });
    pVbFull = VertexBuffer::Resolve(gfx, "$Full_FrameCmd_VB", std::move(vbuf_full));
    std::vector<unsigned short> indices_full = { 0, 1, 2, 1, 3, 2 };
    pIbFull = IndexBuffer::Resolve(gfx, "$Full_FrameCmd_IB", std::move(indices_full));
    pVsFull = VertexShader::Resolve(gfx, "Fullscreen_VS.cso");
    pLayoutFull = InputLayout::Resolve(gfx, lay, pVsFull->GetBytecode());

    // --- Samplers (Using your original ones) ---
    pSamplerFullPoint = Sampler::Resolve(gfx, false, true); // Your original constructor
    pSamplerFullBilin = Sampler::Resolve(gfx, true, true);  // Your original constructor
    // Note: These samplers likely use D3D11_TEXTURE_ADDRESS_WRAP.
    // For motion blur (sampling depth, velocity, scene color), D3D11_TEXTURE_ADDRESS_CLAMP is preferred.
    // If you can modify Sampler::Resolve or create samplers with CLAMP, that would be ideal.
    // If not, WRAP might cause minor artifacts at screen edges for motion blur.

    // --- Blenders ---
    pBlenderMerge = Blender::Resolve(gfx, true); // Your original merge blender
    pBlenderOff = Blender::Resolve(gfx, false);  // Standard opaque

    // --- Motion Blur Shaders & Constant Buffer ---
    psVelocityGen = PixelShader::Resolve(gfx, "VelocityGen_PS.cso");
    psMotionBlurApply = PixelShader::Resolve(gfx, "MotionBlurApply_PS.cso");
    psPassThrough = PixelShader::Resolve(gfx, "Passthrough_PS.cso"); // You need to create this shader
    pCbMotionBlurGPU = std::make_unique<PixelConstantBuffer<MotionBlurConstantsGPU>>(gfx, 1u); // Slot b1

    // --- Create Depth Stencil States ---
    D3D11_DEPTH_STENCIL_DESC dsDesc_Off = {};
    dsDesc_Off.DepthEnable = TRUE;
    dsDesc_Off.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc_Off.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; // Standard depth test
    dsDesc_Off.StencilEnable = FALSE;
    pStencilOffState = CreateDepthStencilState(gfx, dsDesc_Off);

    D3D11_DEPTH_STENCIL_DESC dsDesc_Write = {}; // For Stencil::Mode::Write
    dsDesc_Write.DepthEnable = FALSE; // No depth test/write when only doing stencil
    dsDesc_Write.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc_Write.StencilEnable = TRUE;
    dsDesc_Write.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsDesc_Write.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    dsDesc_Write.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc_Write.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc_Write.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE; // Replace stencil value on pass
    dsDesc_Write.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;    // Always pass func to write
    dsDesc_Write.BackFace = dsDesc_Write.FrontFace; // Same for back face
    pStencilWriteState = CreateDepthStencilState(gfx, dsDesc_Write);

    D3D11_DEPTH_STENCIL_DESC dsDesc_Mask = {}; // For Stencil::Mode::Mask
    dsDesc_Mask.DepthEnable = TRUE; // Depth test usually enabled when drawing with stencil mask
    dsDesc_Mask.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // Or zero if outlines shouldn't write depth
    dsDesc_Mask.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    dsDesc_Mask.StencilEnable = TRUE;
    dsDesc_Mask.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsDesc_Mask.StencilWriteMask = 0; // No stencil writing
    dsDesc_Mask.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc_Mask.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsDesc_Mask.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsDesc_Mask.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL; // Pass if stencil value is equal to ref
    dsDesc_Mask.BackFace = dsDesc_Mask.FrontFace;
    pStencilMaskState = CreateDepthStencilState(gfx, dsDesc_Mask);

    // pNullPsForStencil = CreateNullPixelShader(gfx); // See note above
}

void FrameCommander::Accept(Job job, size_t target) noexcept
{
    if (target < passes.size())
    {
        passes[target].Accept(job);
    }
}

void FrameCommander::Execute(Graphics& gfx,
    DirectX::FXMMATRIX currentViewProj,
    DirectX::FXMMATRIX prevViewProj)
{
    // --- 0. Update Motion Blur Constant Buffer (if enabled) ---
    if (m_motionBlurSettings.enabled)
    {
        MotionBlurConstantsGPU mbcGPU = {};
        mbcGPU.currentViewProjection = dx::XMMatrixTranspose(currentViewProj);
        mbcGPU.previousViewProjection = dx::XMMatrixTranspose(prevViewProj);
        mbcGPU.inverseCurrentViewProjection = dx::XMMatrixTranspose(dx::XMMatrixInverse(nullptr, currentViewProj));
        mbcGPU.RcpBufferDim = { 1.0f / gfx.GetWidth(), 1.0f / gfx.GetHeight() };
        mbcGPU.blurStrength = m_motionBlurSettings.blurStrength;
        mbcGPU.numSamples = m_motionBlurSettings.numSamples;
        mbcGPU.paddingFor16ByteAlignment = {};
        pCbMotionBlurGPU->Update(gfx, mbcGPU);
    }

    // --- PASS 1: Render Main Scene ---
    // Output: sceneColorRT (color), ds (depth/stencil)
    sceneColorRT->BindAsTarget(gfx, ds);
    sceneColorRT->Clear(gfx);
    ds.Clear(gfx); // Clears depth AND stencil to 0

    GraphicsResource::GetContext(gfx)->OMSetDepthStencilState(pStencilOffState.Get(), 0); // Default stencil off, ref value 0
    pBlenderOff->Bind(gfx);
    passes[PassScene].Execute(gfx);


    // --- Prepare for Outline / Final Output ---
    // The surface that the outline passes operate on needs to contain the scene (blurred or not).
    // In your original code, this was the swap_buffer + ds.

    if (m_motionBlurSettings.enabled)
    {
        // --- PASS 2: Generate Velocity Buffer ---
        pCbMotionBlurGPU->Bind(gfx); // Bind motion blur constants

        velocityRT->BindAsTarget(gfx); // No depth test/writes
        velocityRT->Clear(gfx, { 0.0f, 0.0f, 0.0f, 0.0f });

        ds.BindAsTexture(gfx, 0, true);  // Bind depth buffer from Pass 1 as PS texture t0
        pSamplerFullPoint->Bind(gfx);    // Use your existing point sampler (ideally with CLAMP)

        pVbFull->Bind(gfx);
        pIbFull->Bind(gfx);
        pVsFull->Bind(gfx);
        pLayoutFull->Bind(gfx);
        psVelocityGen->Bind(gfx);
        pBlenderOff->Bind(gfx);
        GraphicsResource::GetContext(gfx)->OMSetDepthStencilState(pStencilOffState.Get(), 0); // No stencil/depth writes

        gfx.DrawIndexed(pIbFull->GetCount());
        // Unbind depth from SRV manually - good practice
        ID3D11ShaderResourceView* nullSRV_ds = nullptr;
        GraphicsResource::GetContext(gfx)->PSSetShaderResources(0, 1, &nullSRV_ds);


        // --- PASS 3: Apply Motion Blur ---
        // Output: Directly to swap_buffer. ds is also bound for UI/final depth.
        gfx.BindSwapBuffer(ds); // This binds swap_buffer as RTV, ds as DSV. Clears RTV.

        pCbMotionBlurGPU->Bind(gfx); // Ensure CB is still bound (or re-bind)

        sceneColorRT->BindAsTexture(gfx, 0);     // Original scene color to t0
        velocityRT->BindAsTexture(gfx, 1);       // Velocity buffer to t1

        pSamplerFullBilin->Bind(gfx); // For scene color (ideally CLAMP)
        pSamplerFullPoint->Bind(gfx); // For velocity (ideally CLAMP) - reuse slot if samplers are bound per-texture

        // Rebind samplers to specific slots if they don't auto-match HLSL `register(sX)`
        // GetContext(gfx)->PSSetSamplers(0, 1, pSamplerFullBilin->GetAddressOf()); // for t0
        // GetContext(gfx)->PSSetSamplers(1, 1, pSamplerFullPoint->GetAddressOf()); // for t1


        psMotionBlurApply->Bind(gfx);
        pBlenderOff->Bind(gfx);
        GraphicsResource::GetContext(gfx)->OMSetDepthStencilState(pStencilOffState.Get(), 0); // Standard depth test for scene

        gfx.DrawIndexed(pIbFull->GetCount());

        ID3D11ShaderResourceView* nullSRVs_mb[2] = { nullptr, nullptr };
        GraphicsResource::GetContext(gfx)->PSSetShaderResources(0, 2, nullSRVs_mb);
    }
    else
    {
        // Motion blur disabled: Blit sceneColorRT to the swap_buffer.
        gfx.BindSwapBuffer(ds); // Clears RTV.

        sceneColorRT->BindAsTexture(gfx, 0);
        pSamplerFullPoint->Bind(gfx); // Use point for 1:1 copy (ideally CLAMP)
        pBlenderOff->Bind(gfx);
        GraphicsResource::GetContext(gfx)->OMSetDepthStencilState(pStencilOffState.Get(), 0);

        psPassThrough->Bind(gfx); // Simple shader: tex.Sample(sampler, uv)

        pVbFull->Bind(gfx); // Ensure fullscreen quad is bound
        pIbFull->Bind(gfx);
        pVsFull->Bind(gfx);
        pLayoutFull->Bind(gfx);
        gfx.DrawIndexed(pIbFull->GetCount());

        ID3D11ShaderResourceView* nullSRV_sc = nullptr;
        GraphicsResource::GetContext(gfx)->PSSetShaderResources(0, 1, &nullSRV_sc);
    }

    // At this point, swap_buffer contains the scene (motion-blurred or original).
    // 'ds' contains depth from Pass 1 and potentially stencil values (though it was cleared before Pass 1).
    // The outline passes now operate on this state.

    // --- ORIGINAL OUTLINE PASSES ---
    // PASS OutlineMask: Write to stencil buffer 'ds'. StencilRef = 1.
    // Target is swap_buffer (color writes ignored by NullPS) + ds (stencil writes).
    // gfx.BindSwapBuffer(ds); // Already bound to this.
    GraphicsResource::GetContext(gfx)->OMSetDepthStencilState(pStencilWriteState.Get(), 1); // Stencil ref = 1
    // Jobs in passes[PassOutlineMask] should use a "null" pixel shader or one that doesn't write color
    // if the intent is *only* to update stencil. If they draw stencil-defining objects, they'll use their own PS.
    // For this pass, a common technique is to re-render desired objects with a PS that returns early or discards,
    // and rely on the StencilPassOp = D3D11_STENCIL_OP_REPLACE.
    passes[PassOutlineMask].Execute(gfx);


    // PASS OutlineDraw: Draw outlines into rt1, using stencil mask (StencilRef = 1).
    rt1->BindAsTarget(gfx); // Bind rt1 as RTV, no DSV (or ds if outlines need depth test)
    rt1->Clear(gfx);
    GraphicsResource::GetContext(gfx)->OMSetDepthStencilState(pStencilMaskState.Get(), 1); // Stencil ref = 1
    // Ensure jobs in passes[PassOutlineDraw] draw the actual outlines
    passes[PassOutlineDraw].Execute(gfx);


    // Blur H-Pass: rt1 -> rt2
    rt2->BindAsTarget(gfx); // No DSV
    rt2->Clear(gfx);
    rt1->BindAsTexture(gfx, 0);
    pSamplerFullPoint->Bind(gfx); // Your original sampler choice for blur input
    GraphicsResource::GetContext(gfx)->OMSetDepthStencilState(pStencilOffState.Get(), 0); // No stencil for blur
    pBlenderOff->Bind(gfx); // Blur is opaque write usually
    blur.Bind(gfx);
    blur.SetHorizontal(gfx);
    gfx.DrawIndexed(pIbFull->GetCount());
    // Unbind rt1
    ID3D11ShaderResourceView* nullSRV_rt1 = nullptr;
    GraphicsResource::GetContext(gfx)->PSSetShaderResources(0, 1, &nullSRV_rt1);


    // Blur V-Pass + Combine: rt2 -> swap_buffer (merging with existing scene)
    gfx.BindSwapBuffer(ds); // Target is final swap_buffer, which has scene. ds for UI depth.
    rt2->BindAsTexture(gfx, 0u);
    pSamplerFullBilin->Bind(gfx); // Your original sampler choice for blur input
    GraphicsResource::GetContext(gfx)->OMSetDepthStencilState(pStencilMaskState.Get(), 1); // Your original stencil for merge
    // Or pStencilOffState if merge doesn't use stencil
    pBlenderMerge->Bind(gfx); // Your original merge blender
    blur.SetVertical(gfx);    // Assumes this shader also reads the background (swap_buffer) or just blends
    gfx.DrawIndexed(pIbFull->GetCount());


    // --- Final State Reset ---
    GraphicsResource::GetContext(gfx)->OMSetDepthStencilState(pStencilOffState.Get(), 0); // Reset to default
    ID3D11ShaderResourceView* nullSRVs_final[1] = { nullptr }; // Max SRVs bound in last step was 1 (rt2)
    GraphicsResource::GetContext(gfx)->PSSetShaderResources(0, 1, nullSRVs_final);
}

void FrameCommander::Reset() noexcept
{
    for (auto& p : passes)
    {
        p.Reset();
    }
}

void FrameCommander::ShowWindows(Graphics& gfx)
{
    // Same as before
    if (ImGui::Begin("Post-Processing Control"))
    {
        if (ImGui::Checkbox("Enable Motion Blur", &m_motionBlurSettings.enabled)) {
            if (!m_motionBlurSettings.enabled && velocityRT.has_value()) {
                velocityRT->Clear(gfx, { 0.0f, 0.0f, 0.0f, 0.0f }); // Clear if disabled
            }
        }

        if (m_motionBlurSettings.enabled) {
            ImGui::SliderFloat("Blur Strength", &m_motionBlurSettings.blurStrength, 0.0f, 2.0f, "%.2f");
            ImGui::SliderInt("Blur Samples", &m_motionBlurSettings.numSamples, 1, 32);
        }

        ImGui::Separator();
        ImGui::Text("Outline Effect:");
        if (ImGui::SliderInt("Outline Down Factor", &downFactor, 1, 16)) { // Used 'downFactor' as in your original
            rt1.emplace(gfx, gfx.GetWidth() / downFactor, gfx.GetHeight() / downFactor);
            rt2.emplace(gfx, gfx.GetWidth() / downFactor, gfx.GetHeight() / downFactor);
        }
        blur.RenderWidgets(gfx);
    }
    ImGui::End();
}
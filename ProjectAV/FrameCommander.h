#pragma once
#include <array>
#include <optional>
#include <DirectXMath.h>
#include "Graphics.h"
#include "Job.h"
#include "Pass.h"
#include "DepthStencil.h"      // Provided
#include "RenderTarget.h"      // Provided
#include "BlurPack.h"          // Provided (from original FrameCommander)
#include "BindableCommon.h"    // For Bind namespace

// Forward declarations for Bind namespace elements
namespace Bind
{
    class VertexBuffer;
    class IndexBuffer;
    class VertexShader;
    class PixelShader;
    class InputLayout;
    class Sampler;
    class Blender;
    template<typename C>
    class PixelConstantBuffer;
}

class FrameCommander
{
public:
    FrameCommander(Graphics& gfx);
    void Accept(Job job, size_t target) noexcept;
    void Execute(Graphics& gfx,
        DirectX::FXMMATRIX currentViewProj,
        DirectX::FXMMATRIX prevViewProj);
    void Reset() noexcept;
    void ShowWindows(Graphics& gfx);

private:
    // Pass indices for job submission
    static constexpr size_t PassScene = 0;
    static constexpr size_t PassOutlineMask = 1;
    static constexpr size_t PassOutlineDraw = 2;

    std::array<Pass, 3> passes;

    int downFactor = 1; // Name from your original FrameCommander for outline blur

    // Core Render Targets
    DepthStencil ds;
    std::optional<RenderTarget> sceneColorRT; // For the main scene if motion blur is on
    std::optional<RenderTarget> velocityRT;   // For motion vectors

    // Resources for outline effect (as in your original FrameCommander)
    std::optional<RenderTarget> rt1;
    std::optional<RenderTarget> rt2;
    BlurPack blur; // Your existing BlurPack

    // Fullscreen quad resources
    std::shared_ptr<Bind::VertexBuffer> pVbFull;
    std::shared_ptr<Bind::IndexBuffer> pIbFull;
    std::shared_ptr<Bind::VertexShader> pVsFull;
    std::shared_ptr<Bind::InputLayout> pLayoutFull;

    // Samplers (as in your original FrameCommander)
    std::shared_ptr<Bind::Sampler> pSamplerFullPoint; // Original
    std::shared_ptr<Bind::Sampler> pSamplerFullBilin; // Original
    // We'll try to use these. For clamping, if these don't clamp, there might be artifacts.

    // Blenders (as in your original FrameCommander)
    std::shared_ptr<Bind::Blender> pBlenderMerge; // Original
    std::shared_ptr<Bind::Blender> pBlenderOff;   // For opaque drawing (new, but standard)

    // Shaders for Motion Blur
    std::shared_ptr<Bind::PixelShader> psVelocityGen;
    std::shared_ptr<Bind::PixelShader> psMotionBlurApply;
    std::shared_ptr<Bind::PixelShader> psPassThrough; // For blitting sceneColorRT

    // Constant Buffer for Motion Blur parameters
    struct MotionBlurConstantsGPU
    {
        DirectX::XMMATRIX currentViewProjection;
        DirectX::XMMATRIX previousViewProjection;
        DirectX::XMMATRIX inverseCurrentViewProjection;
        DirectX::XMFLOAT2 RcpBufferDim;
        float blurStrength;
        int numSamples;
        DirectX::XMFLOAT2 paddingFor16ByteAlignment;
    };
    std::unique_ptr<Bind::PixelConstantBuffer<MotionBlurConstantsGPU>> pCbMotionBlurGPU;

    struct MotionBlurSettings
    {
        float blurStrength = 0.7f;
        int numSamples = 8;
        bool enabled = true;
    } m_motionBlurSettings;

    // For stencil state management - these would be ID3D11DepthStencilState*
    // You'll need to create these states in the constructor.
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pStencilWriteState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pStencilMaskState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pStencilOffState; // Default
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pNullPsForStencil; // A PS that discard()s or writes no color
};
#pragma once
#include "Component.h"
#include "InstanceBuffer.h"
#include "IEmitterLogic.h"
#include <vector>
#include <random>
#include <memory>
#include "ParticleCbuf.h"

// Forward declarations to keep header clean and reduce compile times
class Graphics;
namespace DirectX { struct XMMATRIX; }
namespace Bind
{
    class VertexBuffer;
    class IndexBuffer;
    class InputLayout;
    class VertexShader;
    class PixelShader;
    class Texture;
    class Sampler;
    class Blender;
    template<typename C>
    class VertexConstantBuffer;
}
namespace Rgph
{
    class ParticlePass;
    class RenderGraph;
}

class ParticleSystemComponent : public Component
{
public:
    // Constructor now takes a pointer to an emitter strategy
    ParticleSystemComponent(Node* owner, Graphics& gfx, const std::string& texturePath,
        UINT maxParticles, std::unique_ptr<IEmitterLogic> pEmitterLogic);

    // Standard component functions
    void Update(float dt) override;
    void Submit(Graphics& gfx, DirectX::FXMMATRIX worldTransform) const;

    // Connects this component to the render graph's dedicated particle pass
    void Link(Rgph::RenderGraph& rg);

    // This method is now public so that emitter strategies can call it
    void EmitParticle(const DirectX::XMFLOAT3& position);

    // --- Public Properties ---
    // These properties are accessed by the various emitter strategies and by the particle simulation.

    // Used by PointEmitterLogic
    float EmissionRate = 100.0f;
    // Used by TrailEmitterLogic (and can be customized)
    float ParticlesPerMeter = 50.0f;

    // General particle appearance and behavior
    DirectX::XMFLOAT3 EmitterPositionOffset = { 0.0f, 0.0f, 0.0f };
    float ParticleLifetime = 2.0f;
    DirectX::XMFLOAT3 ParticleVelocity = { 0.0f, 0.5f, 0.0f };
    DirectX::XMFLOAT3 ParticleVelocityVariance = { 0.2f, 0.2f, 0.2f };
    DirectX::XMFLOAT4 StartColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    DirectX::XMFLOAT4 EndColor = { 1.0f, 1.0f, 1.0f, 0.0f };
    float StartSize = 0.5f;
    float EndSize = 0.1f;
    float StartRotation = 0.0f;
    float EndRotation = 3.14159f;

private:
    // Private function containing the actual GPU rendering commands
    void Draw(Graphics& gfx) const;

    // --- Data Structures ---
private:
    // Represents the state of a single particle on the CPU
    struct Particle
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 velocity;

        DirectX::XMFLOAT4 startColor, endColor;
        float startSize, endSize;
        float startRotation, endRotation;

        float lifetime = 0.0f;
        float age = 0.0f;
        bool active = false;
    };

    // The vertex structure for the static quad geometry
    struct ParticleVertex
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT2 tc;
    };

    // The data that will be uploaded to the GPU for each active particle instance
    struct InstanceData
    {
        DirectX::XMFLOAT3 instancePos;
        DirectX::XMFLOAT4 instanceColor;
        DirectX::XMFLOAT2 instanceSize;
        float instanceRot;
    };

    // --- Member Variables ---
private:
    Graphics& gfx;

    // Render Graph Integration
    Rgph::ParticlePass* pTargetPass = nullptr;

    // Emitter Strategy
    std::unique_ptr<IEmitterLogic> pEmitterLogic;

    // CPU-side simulation data
    std::vector<Particle> particles;
    std::vector<InstanceData> instanceData;
    UINT activeParticleCount = 0;
    std::mt19937 rng;

    // GPU-side bindable resources
    std::shared_ptr<Bind::VertexBuffer> pVertexBuffer;
    std::unique_ptr<Bind::InstanceBuffer<InstanceData>> pInstanceBuffer;
    std::shared_ptr<Bind::IndexBuffer> pIndexBuffer;
    std::shared_ptr<Bind::InputLayout> pInputLayout;
    std::shared_ptr<Bind::VertexShader> pVertexShader;
    std::shared_ptr<Bind::PixelShader> pPixelShader;
    std::shared_ptr<Bind::Texture> pTexture;
    std::shared_ptr<Bind::Sampler> pSampler;
    std::shared_ptr<Bind::Blender> pBlender;
    mutable std::unique_ptr<Bind::VertexConstantBuffer<ParticleCbuf>> pVcbuf;
};
#pragma once
#include "Component.h"
#include "InstanceBuffer.h"
#include <vector>
#include <random>
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
    // Constructor is simple, with no need to know about render pass names
    ParticleSystemComponent(Node* owner, Graphics& gfx, const std::string& texturePath, UINT maxParticles);

    // Standard component functions
    void Update(float dt) override;
    void Submit(Graphics& gfx, DirectX::FXMMATRIX worldTransform) const;

    // Connects this component to the render graph's dedicated particle pass
    void Link(Rgph::RenderGraph& rg);

    // Public properties for easy customization of the effect
    DirectX::XMFLOAT3 EmitterPositionOffset = { 0.0f, 0.0f, 0.0f };
    float EmissionRate = 100.0f;
    float ParticleLifetime = 2.0f;
    DirectX::XMFLOAT3 ParticleVelocity = { 0.0f, 5.0f, 0.0f };
    DirectX::XMFLOAT3 ParticleVelocityVariance = { 2.0f, 1.0f, 2.0f };
    DirectX::XMFLOAT4 StartColor = { 1.0f, 0.5f, 0.2f, 1.0f };
    DirectX::XMFLOAT4 EndColor = { 1.0f, 1.0f, 0.8f, 0.0f };
    float StartSize = 1.0f;
    float EndSize = 0.1f;
    float StartRotation = 0.0f;
    float EndRotation = 3.14159f;

private:
    // Helper function to create a new particle
    void EmitParticle(const DirectX::XMFLOAT3& emitterWorldPos);
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
    // This MUST match the InstanceInput struct in the vertex shader.
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

    // CPU-side simulation data
    std::vector<Particle> particles;
    std::vector<InstanceData> instanceData;
    UINT activeParticleCount = 0;
    float timeSinceLastEmission = 0.0f;
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
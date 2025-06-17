#pragma once
#include "Component.h"
#include "InstanceBuffer.h" // Our new class
#include <vector>
#include <random>
#include "ParticleCbuf.h" // Our new CBuffer struct

namespace Bind {
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

class ParticleSystemComponent : public Component
{
public:
    ParticleSystemComponent(Node* owner, Graphics& gfx, const std::string& texturePath, UINT maxParticles);

    void Update(float dt) override;
    // Add a submit method to be called by the node
    void Submit(Graphics& gfx, DirectX::FXMMATRIX worldTransform) const;

    // Particle properties can be exposed here as public members or via getters/setters
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
    void EmitParticle(const DirectX::XMFLOAT3& emitterWorldPos);

    // Particle data structures
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

    // Data for the static quad vertex buffer
    struct ParticleVertex
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT2 tc;
    };

    // Data for the dynamic instance buffer, must match shader
    struct InstanceData
    {
        DirectX::XMFLOAT3 instancePos;
        DirectX::XMFLOAT4 instanceColor;
        DirectX::XMFLOAT2 instanceSize;
        float instanceRot;
    };

private:
    // CPU-side data
    std::vector<Particle> particles;
    std::vector<InstanceData> instanceData;
    UINT activeParticleCount = 0;
    float timeSinceLastEmission = 0.0f;
    Graphics& gfx;
    // Random number generation for variance
    std::mt19937 rng;

    // GPU-side bindables
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
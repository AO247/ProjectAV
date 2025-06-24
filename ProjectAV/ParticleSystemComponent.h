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
    // NEW: Enum to define the playback behavior of the emitter.
    enum class PlaybackMode
    {
        Loop,    // The emitter runs continuously.
        OneShot  // The emitter runs for a specified duration then stops.
    };

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

    // --- NEW: Playback Control ---
    void SetPlaybackMode(PlaybackMode mode);
    PlaybackMode GetPlaybackMode() const;

    // For OneShot mode, this starts or restarts the emission.
    // For Loop mode, this ensures the emitter is running.
    void Play();

    // Forcibly stops the emission of new particles, regardless of mode.
    // Existing particles will continue their simulation until they expire.
    void Stop();

    // Returns true if the emitter is currently trying to create new particles.
    bool IsEmitting() const;
    // Returns true if there are any active particles being simulated/rendered.
    // Useful for knowing when a one-shot effect has completely finished.
    bool IsAlive() const;

    // --- Public Properties ---
    // These properties are accessed by the various emitter strategies and by the particle simulation.

    // Used by PointEmitterLogic
    float EmissionRate = 100.0f;
    // Used by TrailEmitterLogic (and can be customized)
    float ParticlesPerMeter = 50.0f;
    // NEW: Used for OneShot mode to determine how long to emit particles.
    float EmissionDuration = 1.0f;

    // General particle appearance and behavior
    DirectX::XMFLOAT3 EmitterPositionOffset = { 0.0f, 0.0f, 0.0f };
    float ParticleLifetime = 2.0f;
    DirectX::XMFLOAT3 ParticleVelocity = { 0.0f, 0.5f, 0.0f };
    DirectX::XMFLOAT3 ParticleVelocityVariance = { 0.2f, 0.2f, 0.2f };
    DirectX::XMFLOAT4 StartColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    DirectX::XMFLOAT4 EndColor = { 1.0f, 1.0f, 1.0f, 0.0f };
    float StartSize = 1.0f;
    float EndSize = 0.1f;
    float StartSizeVariance = 0.0f;
    float StartRotation = 0.0f;
    float EndRotation = 0.0f;
    bool destroyAfterEmission = false;
    bool lockRotationOnYAxis = false;
    bool bAnimateSize = true;
    UINT textureAtlasRows = 1;
    UINT textureAtlasColumns = 1;
    DirectX::XMFLOAT4 MidColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    bool bOneShotIsBurst = false;
    unsigned int BurstAmount = 50;
    // The point in the particle's lifetime (0.0 to 1.0) at which it reaches the MidColor.
    float ColorMidpoint = 0.5f;
    bool bUseMidColor = false;

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
        DirectX::XMFLOAT2 atlasOffset;
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
        DirectX::XMFLOAT2 atlasOffset;
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

    // NEW: Playback state variables
    PlaybackMode m_playbackMode = PlaybackMode::Loop;
    bool m_isEmitting = true; // Controls if the emitter is active
    float m_emissionTimer = 0.0f; // Timer for one-shot duration
    std::uniform_real_distribution<float> unit_dist;      // For [0, 1]
    std::uniform_real_distribution<float> bilateral_dist; // For [-1, 1]
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
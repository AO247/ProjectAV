#include "ParticleSystemComponent.h"
#include "Node.h"
#include "Graphics.h"
#include "RenderGraph.h"
#include "ParticlePass.h"
#include "BindableCommon.h"
#include "BindableCodex.h"
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "InputLayout.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "Texture.h"
#include "Sampler.h"
#include "Blender.h"
#include "ConstantBuffers.h"
#include <DirectXMath.h>
#include <algorithm>

namespace dx = DirectX;

ParticleSystemComponent::ParticleSystemComponent(Node* owner, Graphics& gfx, const std::string& texturePath,
    UINT maxParticles, std::unique_ptr<IEmitterLogic> pEmitterLogic_in)
    : Component(owner),
    gfx(gfx),
    pEmitterLogic(std::move(pEmitterLogic_in)),
    rng(std::random_device{}()),
    m_playbackMode(PlaybackMode::Loop),
    m_isEmitting(true),
    m_emissionTimer(0.0f)
{
    particles.resize(maxParticles);
    instanceData.resize(maxParticles);

    // Create the static geometry for a single particle (a quad) using the Dvtx system
    Dvtx::VertexLayout vl;
    vl.Append(Dvtx::VertexLayout::Position3D);
    vl.Append(Dvtx::VertexLayout::Texture2D);

    Dvtx::VertexBuffer vbuf(std::move(vl));
    vbuf.EmplaceBack(dx::XMFLOAT3{ -0.5f, -0.5f, 0.0f }, dx::XMFLOAT2{ 0.0f, 1.0f });
    vbuf.EmplaceBack(dx::XMFLOAT3{ 0.5f, -0.5f, 0.0f }, dx::XMFLOAT2{ 1.0f, 1.0f });
    vbuf.EmplaceBack(dx::XMFLOAT3{ -0.5f,  0.5f, 0.0f }, dx::XMFLOAT2{ 0.0f, 0.0f });
    vbuf.EmplaceBack(dx::XMFLOAT3{ 0.5f,  0.5f, 0.0f }, dx::XMFLOAT2{ 1.0f, 0.0f });

    pVertexBuffer = Bind::VertexBuffer::Resolve(gfx, "$particlequad", vbuf);

    const std::vector<unsigned short> indices = { 0, 2, 1, 2, 3, 1 };
    pIndexBuffer = Bind::IndexBuffer::Resolve(gfx, "$particle", indices);

    pInstanceBuffer = std::make_unique<Bind::InstanceBuffer<InstanceData>>(gfx, instanceData);

    pVertexShader = Bind::VertexShader::Resolve(gfx, "Particle_VS.cso");
    pPixelShader = Bind::PixelShader::Resolve(gfx, "Particle_PS.cso");

    const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "INSTANCE_POS",   0, DXGI_FORMAT_R32G32B32_FLOAT,    1, (UINT)offsetof(InstanceData, instancePos),   D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "INSTANCE_COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, (UINT)offsetof(InstanceData, instanceColor), D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "INSTANCE_SIZE",  0, DXGI_FORMAT_R32G32_FLOAT,       1, (UINT)offsetof(InstanceData, instanceSize),  D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "INSTANCE_ROT",   0, DXGI_FORMAT_R32_FLOAT,          1, (UINT)offsetof(InstanceData, instanceRot),   D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    };
    pInputLayout = Bind::InputLayout::Resolve(gfx, ied, *pVertexShader);

    pTexture = Bind::Texture::Resolve(gfx, texturePath, 0);
    pSampler = Bind::Sampler::Resolve(gfx);
    pBlender = Bind::Blender::Resolve(gfx, Bind::Blender::Mode::Alpha);
    pVcbuf = std::make_unique<Bind::VertexConstantBuffer<ParticleCbuf>>(gfx);
}

void ParticleSystemComponent::Link(Rgph::RenderGraph& rg)
{
    assert(pTargetPass == nullptr);
    pTargetPass = &rg.GetParticlePass();
}

void ParticleSystemComponent::Update(float dt)
{
    // --- 1. Emitter Logic ---
    if (m_isEmitting)
    {
        if (pEmitterLogic)
        {
            pEmitterLogic->Update(dt, *this);
        }

        if (m_playbackMode == PlaybackMode::OneShot)
        {
            m_emissionTimer += dt;
            if (m_emissionTimer >= EmissionDuration)
            {
                m_isEmitting = false;
            }
        }
    }

    // --- 2. SIMULATE all existing active particles ---
    activeParticleCount = 0;
    for (auto& p : particles)
    {
        if (!p.active) continue;

        p.age += dt;
        if (p.age >= p.lifetime)
        {
            p.active = false;
            continue;
        }

        dx::XMStoreFloat3(&p.position, dx::XMVectorAdd(dx::XMLoadFloat3(&p.position), dx::XMVectorScale(dx::XMLoadFloat3(&p.velocity), dt)));

        float t = p.age / p.lifetime;

        InstanceData& idata = instanceData[activeParticleCount];
        idata.instancePos = p.position;
        dx::XMStoreFloat4(&idata.instanceColor, dx::XMVectorLerp(dx::XMLoadFloat4(&p.startColor), dx::XMLoadFloat4(&p.endColor), t));
        idata.instanceSize.x = std::lerp(p.startSize, p.endSize, t);
        idata.instanceSize.y = idata.instanceSize.x;
        idata.instanceRot = std::lerp(p.startRotation, p.endRotation, t);

        activeParticleCount++;
    }

    // --- 3. Update the GPU instance buffer ---
    if (activeParticleCount > 0)
    {
        pInstanceBuffer->Update(gfx, instanceData);
    }

    // --- 4. CORRECTED: Self-Destruction Logic ---
    // This check is placed at the end of Update to use the freshly calculated state.
    if (destroyAfterEmission && m_playbackMode == PlaybackMode::OneShot && !m_isEmitting && activeParticleCount == 0)
    {
        // Conditions met:
        // 1. The user has opted in (destroyAfterEmission == true).
        // 2. It's a one-shot effect.
        // 3. The emission period has ended (!m_isEmitting).
        // 4. All emitted particles have completed their lifetime (activeParticleCount == 0).

        // It's safe to destroy the owner Node.
        pOwner->Destroy();

        // We return immediately to prevent any further processing on this component,
        // as its owner is now queued for destruction.
        return;
    }
}

void ParticleSystemComponent::EmitParticle(const DirectX::XMFLOAT3& position)
{
    for (auto& p : particles)
    {
        if (!p.active)
        {
            std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

            p.active = true;
            p.age = 0.0f;
            p.lifetime = ParticleLifetime;

            p.startColor = StartColor;
            p.endColor = EndColor;
            p.startSize = StartSize;
            p.endSize = EndSize;
            p.startRotation = StartRotation;
            p.endRotation = EndRotation;

            dx::XMVECTOR basePos = dx::XMLoadFloat3(&position);
            dx::XMVECTOR offsetPos = dx::XMLoadFloat3(&EmitterPositionOffset);
            dx::XMStoreFloat3(&p.position, dx::XMVectorAdd(basePos, offsetPos));

            dx::XMFLOAT3 randomizedVel = {
                ParticleVelocity.x + ParticleVelocityVariance.x * dist(rng),
                ParticleVelocity.y + ParticleVelocityVariance.y * dist(rng),
                ParticleVelocity.z + ParticleVelocityVariance.z * dist(rng)
            };
            p.velocity = randomizedVel;

            return;
        }
    }
}

void ParticleSystemComponent::Submit(Graphics& gfx, dx::FXMMATRIX worldTransform) const
{
    if (activeParticleCount > 0 && pTargetPass)
    {
        pTargetPass->Accept(Rgph::Job{
            [this](Graphics& gfx_in) {
                this->Draw(gfx_in);
            }
            });
    }
}

void ParticleSystemComponent::Draw(Graphics& gfx) const
{
    ParticleCbuf cbuf;
    cbuf.viewProjection = dx::XMMatrixTranspose(gfx.GetCamera() * gfx.GetProjection());
    const auto view = gfx.GetCamera();
    dx::XMVECTOR determinant;
    const auto invView = dx::XMMatrixInverse(&determinant, view);
    dx::XMStoreFloat3(&cbuf.cameraPosition, invView.r[3]);
    cbuf.lockY = lockRotationOnYAxis;
    pVcbuf->Update(gfx, cbuf);

    pVertexShader->Bind(gfx);
    pPixelShader->Bind(gfx);
    pInputLayout->Bind(gfx);
    pTexture->Bind(gfx);
    pSampler->Bind(gfx);
    pBlender->Bind(gfx);
    pVcbuf->Bind(gfx);
    pIndexBuffer->Bind(gfx);

    const UINT strides[] = { (UINT)sizeof(ParticleVertex), pInstanceBuffer->GetStride() };
    const UINT offsets[] = { 0u, 0u };
    ID3D11Buffer* const pBuffers[] = { pVertexBuffer->Get(), pInstanceBuffer->Get() };
    gfx.GetContext()->IASetVertexBuffers(0u, 2, pBuffers, strides, offsets);

    gfx.GetContext()->DrawIndexedInstanced(pIndexBuffer->GetCount(), activeParticleCount, 0, 0, 0);
}

void ParticleSystemComponent::SetPlaybackMode(PlaybackMode mode)
{
    m_playbackMode = mode;
    if (m_playbackMode == PlaybackMode::Loop)
    {
        m_isEmitting = true;
    }
    else // OneShot
    {
        m_isEmitting = false;
        m_emissionTimer = 0.0f;
    }
}

ParticleSystemComponent::PlaybackMode ParticleSystemComponent::GetPlaybackMode() const
{
    return m_playbackMode;
}

void ParticleSystemComponent::Play()
{
    m_isEmitting = true;
    if (m_playbackMode == PlaybackMode::OneShot)
    {
        m_emissionTimer = 0.0f;
    }
}

void ParticleSystemComponent::Stop()
{
    m_isEmitting = false;
}

bool ParticleSystemComponent::IsEmitting() const
{
    return m_isEmitting;
}

bool ParticleSystemComponent::IsAlive() const
{
    return m_isEmitting || (activeParticleCount > 0);
}
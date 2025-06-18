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

ParticleSystemComponent::ParticleSystemComponent(Node* owner, Graphics& gfx, const std::string& texturePath, UINT maxParticles)
    : Component(owner),
    gfx(gfx),
    rng(std::random_device{}())
{
    particles.resize(maxParticles);
    instanceData.resize(maxParticles);

    // Create the static geometry for a single particle (a quad) using the Dvtx system
    Dvtx::VertexLayout vl;
    vl.Append(Dvtx::VertexLayout::Position3D);
    vl.Append(Dvtx::VertexLayout::Texture2D);

    Dvtx::VertexBuffer vbuf(std::move(vl));
    vbuf.EmplaceBack(dx::XMFLOAT3{ -0.5f, -0.5f, 0.0f }, dx::XMFLOAT2{ 0.0f, 1.0f }); // Bottom-left
    vbuf.EmplaceBack(dx::XMFLOAT3{ 0.5f, -0.5f, 0.0f }, dx::XMFLOAT2{ 1.0f, 1.0f }); // Bottom-right
    vbuf.EmplaceBack(dx::XMFLOAT3{ -0.5f,  0.5f, 0.0f }, dx::XMFLOAT2{ 0.0f, 0.0f }); // Top-left
    vbuf.EmplaceBack(dx::XMFLOAT3{ 0.5f,  0.5f, 0.0f }, dx::XMFLOAT2{ 1.0f, 0.0f }); // Top-right

    pVertexBuffer = Bind::VertexBuffer::Resolve(gfx, "$particlequad", vbuf);

    const std::vector<unsigned short> indices = { 0, 2, 1, 2, 3, 1 };
    pIndexBuffer = Bind::IndexBuffer::Resolve(gfx, "$particle", indices);

    pInstanceBuffer = std::make_unique<Bind::InstanceBuffer<InstanceData>>(gfx, instanceData);

    pVertexShader = Bind::VertexShader::Resolve(gfx, "Particle_VS.cso");
    pPixelShader = Bind::PixelShader::Resolve(gfx, "Particle_PS.cso");

    // Define the input layout for both per-vertex and per-instance data
    const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
    {
        // Per-vertex data from the quad VB (Input Slot 0)
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },

        // Per-instance data from the instance VB (Input Slot 1)
        // We use offsetof() to get the precise byte offset of each member in our C++ struct.
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
    // Ask the render graph for the dedicated particle pass using the virtual getter
    pTargetPass = &rg.GetParticlePass();
}

void ParticleSystemComponent::Update(float dt)
{
    // 1. Update existing particles and collect active ones for rendering
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

    // 2. Emit new particles
    const dx::XMFLOAT3 emitterWorldPos = GetOwner()->GetWorldPosition();
    timeSinceLastEmission += dt;
    const float emissionPeriod = 1.0f / EmissionRate;
    while (timeSinceLastEmission > emissionPeriod)
    {
        EmitParticle(emitterWorldPos);
        timeSinceLastEmission -= emissionPeriod;
    }

    // 3. Update the GPU instance buffer if there are active particles
    if (activeParticleCount > 0)
    {
        pInstanceBuffer->Update(gfx, instanceData);
    }
}

void ParticleSystemComponent::EmitParticle(const DirectX::XMFLOAT3& emitterWorldPos)
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

            dx::XMVECTOR basePos = dx::XMLoadFloat3(&emitterWorldPos);
            dx::XMVECTOR offsetPos = dx::XMLoadFloat3(&EmitterPositionOffset);
            dx::XMStoreFloat3(&p.position, dx::XMVectorAdd(basePos, offsetPos));

            dx::XMFLOAT3 randomizedVel = {
                ParticleVelocity.x + ParticleVelocityVariance.x * dist(rng),
                ParticleVelocity.y + ParticleVelocityVariance.y * dist(rng),
                ParticleVelocity.z + ParticleVelocityVariance.z * dist(rng)
            };
            p.velocity = randomizedVel;

            return; // Particle emitted, exit the function
        }
    }
}

void ParticleSystemComponent::Submit(Graphics& gfx, dx::FXMMATRIX worldTransform) const
{
    // If there are active particles and we have a valid target pass, queue a drawing job.
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
    // Update the constant buffer with camera information
    ParticleCbuf cbuf;
    cbuf.viewProjection = dx::XMMatrixTranspose(gfx.GetCamera() * gfx.GetProjection());
    const auto view = gfx.GetCamera();
    dx::XMVECTOR determinant;
    const auto invView = dx::XMMatrixInverse(&determinant, view);
    dx::XMStoreFloat3(&cbuf.cameraPosition, invView.r[3]);
    pVcbuf->Update(gfx, cbuf);

    // Bind all necessary resources to the pipeline
    pVertexShader->Bind(gfx);
    pPixelShader->Bind(gfx);
    pInputLayout->Bind(gfx);
    pTexture->Bind(gfx);
    pSampler->Bind(gfx);
    pBlender->Bind(gfx);
    pVcbuf->Bind(gfx);
    pIndexBuffer->Bind(gfx);

    // Set both the static quad vertex buffer (slot 0) and the dynamic instance buffer (slot 1)
    const UINT strides[] = { (UINT)sizeof(ParticleVertex), pInstanceBuffer->GetStride() };
    const UINT offsets[] = { 0u, 0u };
    ID3D11Buffer* const pBuffers[] = { pVertexBuffer->Get(), pInstanceBuffer->Get() };
    gfx.GetContext()->IASetVertexBuffers(0u, 2, pBuffers, strides, offsets);

    // Execute the instanced draw call
    gfx.GetContext()->DrawIndexedInstanced(pIndexBuffer->GetCount(), activeParticleCount, 0, 0, 0);
}
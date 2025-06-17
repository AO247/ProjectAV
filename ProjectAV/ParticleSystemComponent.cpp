#include "ParticleSystemComponent.h"
#include "Node.h"
#include "Graphics.h"
#include "BindableCommon.h" // For common bindables
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
    gfx(gfx), // Initialize the new member
    rng(std::random_device{}())
{
    particles.resize(maxParticles);
    instanceData.resize(maxParticles);

    // =======================================================================
    // +++ REPLACEMENT CODE FOR VERTEX BUFFER CREATION +++
    // =======================================================================

    // 1. Define the layout of our particle vertex using your Dvtx system.
    Dvtx::VertexLayout vl;
    vl.Append(Dvtx::VertexLayout::Position3D);
    vl.Append(Dvtx::VertexLayout::Texture2D);

    // 2. Create and populate a Dvtx::VertexBuffer object.
    Dvtx::VertexBuffer vbuf(std::move(vl));
    vbuf.EmplaceBack(dx::XMFLOAT3{ -0.5f, -0.5f, 0.0f }, dx::XMFLOAT2{ 0.0f, 1.0f }); // Bottom-left
    vbuf.EmplaceBack(dx::XMFLOAT3{ 0.5f, -0.5f, 0.0f }, dx::XMFLOAT2{ 1.0f, 1.0f }); // Bottom-right
    vbuf.EmplaceBack(dx::XMFLOAT3{ -0.5f,  0.5f, 0.0f }, dx::XMFLOAT2{ 0.0f, 0.0f }); // Top-left
    vbuf.EmplaceBack(dx::XMFLOAT3{ 0.5f,  0.5f, 0.0f }, dx::XMFLOAT2{ 1.0f, 0.0f }); // Top-right

    // 3. Use the Codex to Resolve the Bind::VertexBuffer. This is the correct pattern.
    // The tag "$particlequad" ensures that if multiple particle systems are created,
    // they all share the same static quad geometry, which is efficient.
    pVertexBuffer = Bind::VertexBuffer::Resolve(gfx, "$particlequad", vbuf);

    // =======================================================================
    // --- END OF REPLACEMENT CODE ---
    // =======================================================================

    const std::vector<unsigned short> indices =
    {
        0, 2, 1, 2, 3, 1
    };
    pIndexBuffer = Bind::IndexBuffer::Resolve(gfx, "$particle", indices);

    // Create the dynamic instance buffer
    pInstanceBuffer = std::make_unique<Bind::InstanceBuffer<InstanceData>>(gfx, instanceData);

    // Resolve shaders
    pVertexShader = Bind::VertexShader::Resolve(gfx, "Particle_VS.cso");
    pPixelShader = Bind::PixelShader::Resolve(gfx, "Particle_PS.cso");

    // Create the input layout
    const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
    {
        // Per-vertex data (from the quad VB)
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

        // Per-instance data (from the instance VB)
        { "INSTANCE_POS",   0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "INSTANCE_COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "INSTANCE_SIZE",  0, DXGI_FORMAT_R32G32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "INSTANCE_ROT",   0, DXGI_FORMAT_R32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    };
    pInputLayout = Bind::InputLayout::Resolve(gfx, ied, *pVertexShader);

    // Resolve other bindables
    pTexture = Bind::Texture::Resolve(gfx, texturePath, 0);
    pSampler = Bind::Sampler::Resolve(gfx);
    // Use alpha blending
    pBlender = Bind::Blender::Resolve(gfx, true);

    pVcbuf = std::make_unique<Bind::VertexConstantBuffer<ParticleCbuf>>(gfx);
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

        // Update position
        dx::XMStoreFloat3(&p.position, dx::XMVectorAdd(dx::XMLoadFloat3(&p.position), dx::XMVectorScale(dx::XMLoadFloat3(&p.velocity), dt)));

        // Lerp factor
        float t = p.age / p.lifetime;

        // Build instance data for this particle
        InstanceData& idata = instanceData[activeParticleCount];
        idata.instancePos = p.position;
        dx::XMStoreFloat4(&idata.instanceColor, dx::XMVectorLerp(dx::XMLoadFloat4(&p.startColor), dx::XMLoadFloat4(&p.endColor), t));
        idata.instanceSize.x = std::lerp(p.startSize, p.endSize, t);
        idata.instanceSize.y = idata.instanceSize.x; // Assuming uniform scaling
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

void ParticleSystemComponent::EmitParticle(const dx::XMFLOAT3& emitterWorldPos)
{
    // Find an inactive particle slot
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
void DebugPrintXMMATRIX(const DirectX::XMMATRIX& matrix)
{
    std::wstringstream ss;

    for (int row = 0; row < 4; ++row)
    {
        ss << L"[ ";
        for (int col = 0; col < 4; ++col)
        {
            ss << matrix.r[row].m128_f32[col] << L" ";
        }
        ss << L"]\n";
    }

    OutputDebugStringW(ss.str().c_str());
}
void ParticleSystemComponent::Submit(Graphics& gfx, dx::FXMMATRIX worldTransform) const
{
    if (activeParticleCount == 0) return;

    // Update constant buffer
    ParticleCbuf cbuf;
    cbuf.viewProjection = dx::XMMatrixTranspose(gfx.GetCamera() * gfx.GetProjection());
    // This needs the actual camera position, which might need to be retrieved from your Camera class.
    // For now, let's assume a simple way to get it from the view matrix.
    const auto view = gfx.GetCamera();
    //DebugPrintXMMATRIX(view);
    dx::XMVECTOR determinant;
    const auto invView = dx::XMMatrixInverse(&determinant, view);
    dx::XMStoreFloat3(&cbuf.cameraPosition, invView.r[3]);
    pVcbuf->Update(gfx, cbuf);

    // Bind all necessary resources
    pVertexShader->Bind(gfx);
    pPixelShader->Bind(gfx);
    pInputLayout->Bind(gfx);
    pTexture->Bind(gfx);
    pSampler->Bind(gfx);
    pBlender->Bind(gfx);
    pVcbuf->Bind(gfx);
    pIndexBuffer->Bind(gfx);

    // Set vertex buffers: Slot 0 for quad, Slot 1 for instances
    const UINT strides[] = { (UINT)sizeof(ParticleVertex), pInstanceBuffer->GetStride() };
    const UINT offsets[] = { 0u, 0u };
    ID3D11Buffer* const pBuffers[] = { pVertexBuffer->Get(), pInstanceBuffer->Get() };
    gfx.GetContext()->IASetVertexBuffers(0u, 2, pBuffers, strides, offsets);

    // Draw the instanced particles
    gfx.GetContext()->DrawIndexedInstanced(pIndexBuffer->GetCount(), activeParticleCount, 0, 0, 0);
}
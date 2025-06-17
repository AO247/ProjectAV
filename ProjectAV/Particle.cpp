#include "Particle.h"
#include "BindableCommon.h"
#include "InstanceBuffer.h"
#include "ParticlePass.h"
#include "RenderGraph.h"
#include "Vertex.h"
#include "Texture.h"
#include <random>

namespace dx = DirectX;

ParticleSystem::ParticleSystem(Graphics& gfx, UINT maxParticles, std::string targetPassName)
	:
	maxParticles(maxParticles),
	poolIndex(maxParticles - 1),
	targetPassName(std::move(targetPassName)) // <<< ADD THIS INITIALIZATION
{
	particlePool.resize(maxParticles);
	instanceData.resize(maxParticles);

	pTopology = Bind::Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pSampler = Bind::Sampler::Resolve(gfx);
	pBlender = Bind::Blender::Resolve(gfx, true); // Enable blending

	// ASK: You must provide this texture in your project directory
	pTexture = Bind::Texture::Resolve(gfx, "Images\\particle.png");

	// Create a single quad as the base mesh for all particles
	Dvtx::VertexLayout layout;
	layout.Append(Dvtx::VertexLayout::Position3D);
	layout.Append(Dvtx::VertexLayout::Texture2D);
	Dvtx::VertexBuffer vertices(layout);
	vertices.EmplaceBack(dx::XMFLOAT3{ -0.5f, -0.5f, 0.0f }, dx::XMFLOAT2{ 0.0f, 1.0f });
	vertices.EmplaceBack(dx::XMFLOAT3{ 0.5f, -0.5f, 0.0f }, dx::XMFLOAT2{ 1.0f, 1.0f });
	vertices.EmplaceBack(dx::XMFLOAT3{ -0.5f,  0.5f, 0.0f }, dx::XMFLOAT2{ 0.0f, 0.0f });
	vertices.EmplaceBack(dx::XMFLOAT3{ 0.5f,  0.5f, 0.0f }, dx::XMFLOAT2{ 1.0f, 0.0f });

	pVertexBuffer = std::make_shared<Bind::VertexBuffer>(gfx, vertices);

	std::vector<unsigned short> indices = { 0, 2, 1, 2, 3, 1 };
	pIndexBuffer = std::make_shared<Bind::IndexBuffer>(gfx, indices);

	// The shaders for particles
	pVertexShader = Bind::VertexShader::Resolve(gfx, "Particle_VS.cso");
	pPixelShader = Bind::PixelShader::Resolve(gfx, "Particle_PS.cso");

	pInstanceBuffer = std::make_unique<Bind::InstanceBuffer>(gfx, instanceData);

	// Create a combined input layout: per-vertex for the quad, per-instance for particle data
	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "InstancePos", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "InstanceSize", 0, DXGI_FORMAT_R32G32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "InstanceColor", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};
	pInputLayout = std::make_shared<Bind::InputLayout>(gfx, ied, *pVertexShader);

	pVcbuf = std::make_unique<Bind::VertexConstantBuffer<dx::XMMATRIX>>(gfx);
}

void ParticleSystem::Submit(Rgph::RenderGraph& rg)
{
	// Find the ParticlePass in the render graph and register this system instance
	// This will throw if the pass doesn't exist or isn't a ParticlePass, which is good.
	dynamic_cast<Rgph::ParticlePass&>(rg.GetPass(targetPassName)).Accept(this);
}

void ParticleSystem::Update(float dt)
{
	activeParticleCount = 0;
	for (auto& p : particlePool)
	{
		if (!p.active)
			continue;

		if (p.lifeRemaining <= 0.0f)
		{
			p.active = false;
			continue;
		}

		p.lifeRemaining -= dt;
		p.position.x += p.velocity.x * dt;
		p.position.y += p.velocity.y * dt;
		p.position.z += p.velocity.z * dt;

		// Lerp color and size
		float life = p.lifeRemaining / p.lifeTime; // 1 to 0
		dx::XMVECTOR color = dx::XMVectorLerp(dx::XMLoadFloat4(&p.colorEnd), dx::XMLoadFloat4(&p.colorBegin), life);
		float size = std::lerp(p.sizeEnd, p.sizeBegin, life);

		// Copy data to be sent to GPU
		ParticleInstanceData& data = instanceData[activeParticleCount];
		data.position = p.position;
		dx::XMStoreFloat4(&data.color, color);
		data.size = { size, size };

		activeParticleCount++;
	}
}

void ParticleSystem::Emit(const ParticleProps& props)
{
	Particle& particle = particlePool[poolIndex];
	particle.active = true;
	particle.position = props.position;

	// Velocity
	particle.velocity.x = props.velocity.x + props.velocityVariation.x * (rand() / (float)RAND_MAX * 2.0f - 1.0f);
	particle.velocity.y = props.velocity.y + props.velocityVariation.y * (rand() / (float)RAND_MAX * 2.0f - 1.0f);
	particle.velocity.z = props.velocity.z + props.velocityVariation.z * (rand() / (float)RAND_MAX * 2.0f - 1.0f);

	// Color
	particle.colorBegin = props.colorBegin;
	particle.colorEnd = props.colorEnd;

	// Lifetime
	particle.lifeTime = props.lifeTime;
	particle.lifeRemaining = props.lifeTime;

	// Size
	particle.sizeBegin = props.sizeBegin;
	particle.sizeEnd = props.sizeEnd;

	poolIndex = --poolIndex % maxParticles;
}


void ParticleSystem::Bind(Graphics& gfx) const
{
	pInstanceBuffer->Update(gfx, instanceData); // Send latest instance data to GPU

	const auto view = gfx.GetCamera();
	pVcbuf->Update(gfx, dx::XMMatrixTranspose(view));
	pVcbuf->Bind(gfx);

	pVertexBuffer->Bind(gfx);
	pIndexBuffer->Bind(gfx);
	pInstanceBuffer->Bind(gfx);
	pInputLayout->Bind(gfx);
	pTopology->Bind(gfx);
	pVertexShader->Bind(gfx);
	pPixelShader->Bind(gfx);
	pTexture->Bind(gfx);
	pSampler->Bind(gfx);
	pBlender->Bind(gfx);
}

UINT ParticleSystem::GetInstanceCount() const
{
	return activeParticleCount;
}

UINT ParticleSystem::GetIndexCount() const
{
	return pIndexBuffer->GetCount();
}
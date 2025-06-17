#pragma once
#include "Graphics.h"
#include "Bindable.h"
#include <vector>
#include <memory>
#include "Drawable.h" // For GetTransformXM
#include "ConstantBuffers.h"
#include "Texture.h"

// Forward declarations
namespace Bind
{
	class InstanceBuffer;
	class VertexBuffer;
	class IndexBuffer;
	class Topology;
	class VertexShader;
	class PixelShader;
	class InputLayout;
	class Sampler;
	class Blender;
}

// Properties to define how a particle is emitted
struct ParticleProps
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 velocity;
	DirectX::XMFLOAT3 velocityVariation;
	DirectX::XMFLOAT4 colorBegin, colorEnd;
	float sizeBegin, sizeEnd;
	float lifeTime = 1.0f;
};

class ParticleSystem
{
private:
	// The data for a single particle processed by the CPU
	struct Particle
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 velocity;
		DirectX::XMFLOAT4 colorBegin, colorEnd;
		float rotation = 0.0f;
		float sizeBegin, sizeEnd;
		float lifeTime = 1.0f;
		float lifeRemaining = 0.0f;
		bool active = false;
	};

	// The data that gets sent to the GPU for each instance
	struct ParticleInstanceData
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 size;
		DirectX::XMFLOAT4 color;
		// Add other properties like rotation if needed
	};

public:
	ParticleSystem(Graphics& gfx, UINT maxParticles, std::string targetPassName);
	void Update(float dt);
	void Submit(Rgph::RenderGraph& rg); // The new submission function
	void Emit(const ParticleProps& particleProps);
	void Bind(Graphics& gfx) const;
	UINT GetInstanceCount() const;
	UINT GetIndexCount() const;

private:
	// CPU-side particle pool
	std::vector<Particle> particlePool;
	UINT poolIndex;
	UINT maxParticles;
	std::string targetPassName;
	// GPU-side data
	std::vector<ParticleInstanceData> instanceData;
	UINT activeParticleCount = 0;

	// Bindables for the particle quad
	std::shared_ptr<Bind::VertexBuffer> pVertexBuffer;
	std::shared_ptr<Bind::IndexBuffer> pIndexBuffer;
	std::unique_ptr<Bind::InstanceBuffer> pInstanceBuffer;
	std::shared_ptr<Bind::InputLayout> pInputLayout;

	// Common bindables
	std::shared_ptr<Bind::Topology> pTopology;
	std::shared_ptr<Bind::VertexShader> pVertexShader;
	std::shared_ptr<Bind::PixelShader> pPixelShader;
	std::shared_ptr<Bind::Sampler> pSampler;
	std::shared_ptr<Bind::Blender> pBlender;
	std::shared_ptr<Bind::Texture> pTexture;

	// For billboarding
	std::unique_ptr<Bind::VertexConstantBuffer<DirectX::XMMATRIX>> pVcbuf;
};
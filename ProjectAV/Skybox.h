#pragma once
#include "Graphics.h"
#include <vector>
#include <string>
#include <memory>
#include <wrl.h>
#include "ConstantBuffers.h"

namespace Bind
{
	class IndexBuffer;
	class VertexBuffer;
	class VertexShader;
	class PixelShader;
	class InputLayout;
	class Topology;
	class Sampler;
	class Rasterizer;
	template<typename C>
	class ConstantBuffer;
}

struct SkyboxTransforms;

class Skybox
{
public:
	Skybox(Graphics& gfx, const std::string& texturePath);
	void Draw(Graphics& gfx) const;

private:
	std::shared_ptr<Bind::VertexBuffer> pVertices;
	std::shared_ptr<Bind::IndexBuffer> pIndices;
	std::shared_ptr<Bind::VertexShader> pVertexShader;
	std::shared_ptr<Bind::PixelShader> pPixelShader;
	std::shared_ptr<Bind::InputLayout> pInputLayout;
	std::shared_ptr<Bind::Topology> pTopology;
	std::shared_ptr<Bind::Sampler> pSampler;
	std::shared_ptr<Bind::Rasterizer> pRasterizer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pDepthStencilState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
	std::unique_ptr<Bind::VertexConstantBuffer<SkyboxTransforms>> pVcbuf;
};
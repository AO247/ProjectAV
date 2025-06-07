#include "Skybox.h"
#include "BindableCommon.h"
#include "Vertex.h"
#include "Surface.h"
#include "GraphicsThrowMacros.h"
#include "ConstantBuffers.h"
#include "Sphere.h"
#include "wrl.h"

#ifndef NDEBUG
#define INFOMAN(gfx) DxgiInfoManager& infoManager = gfx.GetInfoManager()
#else
#define INFOMAN(gfx)
#endif

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

// Local struct for the constant buffer
struct SkyboxTransforms
{
	dx::XMMATRIX view;
	dx::XMMATRIX projection;
};

Skybox::Skybox(Graphics& gfx, const std::vector<std::string>& facePaths)
{
	using namespace Bind;
	namespace dvtx = Dvtx;

	if (facePaths.size() != 6)
	{
		throw std::runtime_error("Skybox requires 6 texture face paths.");
	}

	// Use a sphere for seamless geometry
	auto model = Sphere::Make(dvtx::VertexLayout{}.Append(dvtx::VertexLayout::Position3D));
	pVertices = VertexBuffer::Resolve(gfx, "$skybox_v", model.vertices);
	pIndices = IndexBuffer::Resolve(gfx, "$skybox_i", model.indices);
	pTopology = Topology::Resolve(gfx);

	auto pvs = VertexShader::Resolve(gfx, "Skybox_VS.cso");
	pPixelShader = PixelShader::Resolve(gfx, "Skybox_PS.cso");
	pInputLayout = InputLayout::Resolve(gfx, model.vertices.GetLayout(), *pvs);
	pVertexShader = std::move(pvs);

	pVcbuf = std::make_unique<VertexConstantBuffer<SkyboxTransforms>>(gfx, 0u);

	pSampler = Sampler::Resolve(gfx, Sampler::Type::Bilinear);
	// Flip the sphere inside out so back-face culling works from the inside
	pRasterizer = Rasterizer::Resolve(gfx, true);

	// === Create custom Depth Stencil State (as requested) ===
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // Don't write to depth buffer
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;      // Draw if pixel is behind or at same depth
	dsDesc.StencilEnable = FALSE;
	INFOMAN(gfx);
	HRESULT hr;
	D3D11_RASTERIZER_DESC rasterDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
	rasterDesc.CullMode = D3D11_CULL_FRONT; // Cull front faces as we are inside the sphere
	wrl::ComPtr<ID3D11RasterizerState> tempRasterizer;
	GFX_THROW_INFO(gfx.GetDevice()->CreateRasterizerState(&rasterDesc, &tempRasterizer));
	pRasterizer = std::make_shared<Bind::Rasterizer>(gfx, tempRasterizer.Get());
	GFX_THROW_INFO(gfx.GetDevice()->CreateDepthStencilState(&dsDesc, &pDepthStencilState));

	// Load cubemap texture
	std::vector<Surface> surfaces;
	surfaces.reserve(6);
	for (const auto& path : facePaths)
	{
		surfaces.push_back(Surface::FromFile(path));
	}

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = surfaces[0].GetWidth();
	textureDesc.Height = surfaces[0].GetHeight();
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 6;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	D3D11_SUBRESOURCE_DATA subresourceData[6];
	for (int i = 0; i < 6; i++)
	{
		subresourceData[i].pSysMem = surfaces[i].GetBufferPtrConst();
		subresourceData[i].SysMemPitch = surfaces[i].GetWidth() * sizeof(Surface::Color);
		subresourceData[i].SysMemSlicePitch = 0;
	}

	wrl::ComPtr<ID3D11Texture2D> pTexture;
	GFX_THROW_INFO(gfx.GetDevice()->CreateTexture2D(&textureDesc, subresourceData, &pTexture));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = 1;
	GFX_THROW_INFO(gfx.GetDevice()->CreateShaderResourceView(pTexture.Get(), &srvDesc, &pTextureView));
}

void Skybox::Draw(Graphics& gfx) const
{
	// Update constant buffer
	const SkyboxTransforms tf = {
		dx::XMMatrixTranspose(gfx.GetCamera()),
		dx::XMMatrixTranspose(gfx.GetProjection())
	};
	pVcbuf->Update(gfx, tf);

	// Bind all resources
	pVcbuf->Bind(gfx);
	pVertices->Bind(gfx);
	pIndices->Bind(gfx);
	pVertexShader->Bind(gfx);
	pPixelShader->Bind(gfx);
	pInputLayout->Bind(gfx);
	pTopology->Bind(gfx);
	pSampler->Bind(gfx);
	pRasterizer->Bind(gfx);

	// Bind custom resources
	gfx.GetContext()->OMSetDepthStencilState(pDepthStencilState.Get(), 0xFF);
	gfx.GetContext()->PSSetShaderResources(0, 1, pTextureView.GetAddressOf());

	// Draw the geometry
	gfx.DrawIndexed(pIndices->GetCount());
}
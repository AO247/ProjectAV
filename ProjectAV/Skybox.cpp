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


struct SkyboxTransforms
{
	dx::XMMATRIX view;
	dx::XMMATRIX projection;
};

Skybox::Skybox(Graphics& gfx, const std::string& texturePath)
{
	using namespace Bind;
	namespace dvtx = Dvtx;

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

	HRESULT hr;
	INFOMAN(gfx);
	D3D11_RASTERIZER_DESC rasterDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
	rasterDesc.CullMode = D3D11_CULL_FRONT;
	pRasterizer = Rasterizer::Resolve(gfx, true);

	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dsDesc.StencilEnable = FALSE;
	GFX_THROW_INFO(gfx.GetDevice()->CreateDepthStencilState(&dsDesc, &pDepthStencilState));

	const auto s = Surface::FromFile(texturePath);

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = s.GetWidth();
	textureDesc.Height = s.GetHeight();
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0; 

	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = s.GetBufferPtrConst();
	sd.SysMemPitch = s.GetWidth() * sizeof(Surface::Color);

	wrl::ComPtr<ID3D11Texture2D> pTexture;
	GFX_THROW_INFO(gfx.GetDevice()->CreateTexture2D(&textureDesc, &sd, &pTexture));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	GFX_THROW_INFO(gfx.GetDevice()->CreateShaderResourceView(pTexture.Get(), &srvDesc, &pTextureView));
}


void Skybox::Draw(Graphics& gfx) const
{

	const SkyboxTransforms tf = {
		dx::XMMatrixTranspose(gfx.GetCamera()),
		dx::XMMatrixTranspose(gfx.GetProjection())
	};
	pVcbuf->Update(gfx, tf);


	pVcbuf->Bind(gfx);
	pVertices->Bind(gfx);
	pIndices->Bind(gfx);
	pVertexShader->Bind(gfx);
	pPixelShader->Bind(gfx);
	pInputLayout->Bind(gfx);
	pTopology->Bind(gfx);
	pSampler->Bind(gfx);
	pRasterizer->Bind(gfx);


	gfx.GetContext()->OMSetDepthStencilState(pDepthStencilState.Get(), 0xFF);
	gfx.GetContext()->PSSetShaderResources(0, 1, pTextureView.GetAddressOf());

	gfx.DrawIndexed(pIndices->GetCount());
}
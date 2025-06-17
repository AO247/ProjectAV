#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"
#include <vector>

namespace Bind
{
	class InstanceBuffer : public Bindable
	{
	public:
		template<typename T>
		InstanceBuffer(Graphics& gfx, const std::vector<T>& instances)
			:
			stride(sizeof(T))
		{
			INFOMAN(gfx);
			D3D11_BUFFER_DESC bd = {};
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bd.MiscFlags = 0u;
			bd.ByteWidth = UINT(stride * instances.size());
			bd.StructureByteStride = stride;

			D3D11_SUBRESOURCE_DATA sd = {};
			sd.pSysMem = instances.data();
			GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bd, &sd, &pInstanceBuffer));
		}

		template<typename T>
		void Update(Graphics& gfx, const std::vector<T>& instances)
		{
			INFOMAN(gfx);
			D3D11_MAPPED_SUBRESOURCE msr;
			GFX_THROW_INFO(GetContext(gfx)->Map(
				pInstanceBuffer.Get(), 0u,
				D3D11_MAP_WRITE_DISCARD, 0u,
				&msr
			));
			memcpy(msr.pData, instances.data(), instances.size() * stride);
			GetContext(gfx)->Unmap(pInstanceBuffer.Get(), 0u);
		}

		void Bind(Graphics& gfx) noxnd override
		{
			const UINT offset = 0u;
			INFOMAN_NOHR(gfx);
			// Bind to slot 1, leaving slot 0 for the per-vertex data of the quad
			GFX_THROW_INFO_ONLY(GetContext(gfx)->IASetVertexBuffers(1u, 1u, pInstanceBuffer.GetAddressOf(), &stride, &offset));
		}
	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pInstanceBuffer;
		UINT stride;
	};
}
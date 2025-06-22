#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	template<typename T>
	class InstanceBuffer : public Bindable
	{
	public:
		InstanceBuffer(Graphics& gfx, const std::vector<T>& instances)
		{
			INFOMAN(gfx);

			D3D11_BUFFER_DESC bd = {};
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bd.MiscFlags = 0u;
			bd.ByteWidth = UINT(sizeof(T) * instances.size());
			bd.StructureByteStride = sizeof(T);

			D3D11_SUBRESOURCE_DATA sd = {};
			sd.pSysMem = instances.data();

			GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bd, &sd, &pInstanceBuffer));
		}

		void Update(Graphics& gfx, const std::vector<T>& instances)
		{
			INFOMAN(gfx);

			D3D11_MAPPED_SUBRESOURCE msr;
			GFX_THROW_INFO(GetContext(gfx)->Map(
				pInstanceBuffer.Get(), 0u,
				D3D11_MAP_WRITE_DISCARD, 0u,
				&msr
			));
			memcpy(msr.pData, instances.data(), sizeof(T) * instances.size());
			GetContext(gfx)->Unmap(pInstanceBuffer.Get(), 0u);
		}

		// This Bind is a placeholder; actual binding happens with IASetVertexBuffers
		void Bind(Graphics& gfx) noxnd override
		{
			// The actual binding will be done in the component's submit call
			// using IASetVertexBuffers with multiple slots.
		}

		ID3D11Buffer* Get() const
		{
			return pInstanceBuffer.Get();
		}

		UINT GetStride() const
		{
			return sizeof(T);
		}

	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pInstanceBuffer;
	};
}
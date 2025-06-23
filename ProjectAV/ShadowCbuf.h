#pragma once
#include "ConstantBuffers.h"
#include <DirectXMath.h>

class Drawable;
class DirectionalLight;

namespace Bind
{
	class ShadowCbuf : public CloningBindable
	{
	protected:
		struct Transforms
		{
			DirectX::XMMATRIX lightTransform;
		};
	public:
		ShadowCbuf(Graphics& gfx, UINT slot = 0u);
		void Bind(Graphics& gfx) noxnd override;
		void InitializeParentReference(const Drawable& parent) noexcept override;
		std::unique_ptr<CloningBindable> Clone() const noexcept override;
		static void SetLight(const DirectionalLight* pLight);

	protected:
		void UpdateBindImpl(Graphics& gfx, const Transforms& tf) noxnd;
		Transforms GetTransforms(Graphics& gfx) noxnd;

	private:
		ShadowCbuf() noexcept;
		const Drawable* pParent = nullptr;
		static const DirectionalLight* pLight;
		static std::unique_ptr<VertexConstantBuffer<Transforms>> pVcbuf;
		UINT slot;
	};
}
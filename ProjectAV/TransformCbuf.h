#pragma once
#include "ConstantBuffers.h"
#include <DirectXMath.h>

class Drawable;
class DirectionalLight; // Forward declaration

namespace Bind
{
	class TransformCbuf : public CloningBindable
	{
	protected:
		struct Transforms
		{
			DirectX::XMMATRIX modelView;
			DirectX::XMMATRIX modelViewProj;
			DirectX::XMMATRIX lightTransform;
		};
	public:
		TransformCbuf(Graphics& gfx, UINT slot = 0u);
		void Bind(Graphics& gfx) noxnd override;
		void InitializeParentReference(const Drawable& parent) noexcept override;
		std::unique_ptr<CloningBindable> Clone() const noexcept override;
		// --- DODAJ TÊ FUNKCJÊ ---
		static void SetLight(const DirectionalLight* pLight);
	protected:
		void UpdateBindImpl(Graphics& gfx, const Transforms& tf) noxnd;
		Transforms GetTransforms(Graphics& gfx) noxnd;
	private:
		// ZMIANA: pVcbuf nie jest ju¿ unikalny dla tej klasy, jest wspó³dzielony
		static std::unique_ptr<VertexConstantBuffer<Transforms>> pVcbuf;
		const Drawable* pParent = nullptr;
		// --- DODAJ TO POLE ---
		static const DirectionalLight* pLight;
	};
}
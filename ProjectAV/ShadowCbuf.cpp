#include "ShadowCbuf.h"
#include "DirectionalLight.h"
#include "Drawable.h"

namespace Bind
{
	// ZMIANA: Inicjalizacja statycznych wskaŸników
	std::unique_ptr<VertexConstantBuffer<ShadowCbuf::Transforms>> ShadowCbuf::pVcbuf;
	const DirectionalLight* ShadowCbuf::pLight = nullptr;

	ShadowCbuf::ShadowCbuf(Graphics& gfx, UINT slot)
	{
		// Inicjalizujemy bufor tylko raz, przy pierwszym tworzeniu obiektu
		if (!pVcbuf)
		{
			pVcbuf = std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot);
		}
	}

	void ShadowCbuf::Bind(Graphics& gfx) noxnd
	{
		UpdateBindImpl(gfx, GetTransforms(gfx));
	}

	void ShadowCbuf::UpdateBindImpl(Graphics& gfx, const Transforms& tf) noxnd
	{
		pVcbuf->Update(gfx, tf);
		pVcbuf->Bind(gfx);
	}

	ShadowCbuf::Transforms ShadowCbuf::GetTransforms(Graphics& gfx) noxnd
	{
		assert(pParent != nullptr);
		assert(pLight != nullptr && "Light pointer in ShadowCbuf has not been set!");
		return {
			DirectX::XMMatrixTranspose(
				pParent->GetTransformXM() * pLight->GetViewMatrix() * pLight->GetProjectionMatrix()
			)
		};
	}

	void ShadowCbuf::InitializeParentReference(const Drawable& parent) noexcept
	{
		pParent = &parent;
	}

	std::unique_ptr<CloningBindable> ShadowCbuf::Clone() const noexcept
	{
		// Teraz to zadzia³a, bo domyœlny konstruktor kopiuj¹cy nie musi kopiowaæ `unique_ptr`
		return std::make_unique<ShadowCbuf>(*this);
	}

	void ShadowCbuf::SetLight(const DirectionalLight* pLight_in)
	{
		// Ustawiamy statyczny wskaŸnik na œwiat³o dla WSZYSTKICH instancji ShadowCbuf
		pLight = pLight_in;
	}
}
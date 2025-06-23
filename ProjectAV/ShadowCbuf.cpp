#include "ShadowCbuf.h"
#include "DirectionalLight.h"
#include "Drawable.h"

namespace Bind
{
	// ZMIANA: Inicjalizacja statycznych wska�nik�w
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
	ShadowCbuf::ShadowCbuf() noexcept
		: slot(0u) // Slot jest dziedziczony z pierwszego obiektu, ale tutaj mo�na ustawi� domy�lny
	{
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
		auto pClone = std::unique_ptr<ShadowCbuf>(new ShadowCbuf());
		pClone->slot = this->slot; // Klon musi wiedzie� na kt�rym slocie ma dzia�a�
		return pClone;
	}

	void ShadowCbuf::SetLight(const DirectionalLight* pLight_in)
	{
		// Ustawiamy statyczny wska�nik na �wiat�o dla WSZYSTKICH instancji ShadowCbuf
		pLight = pLight_in;
	}
}
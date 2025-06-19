#include "TransformCbuf.h"
#include "DirectionalLight.h" // Pe�na definicja
#include "Drawable.h"       // Pe�na definicja

namespace Bind
{
	// --- DODAJ INICJALIZACJ� ---
	std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::pVcbuf;
	const DirectionalLight* TransformCbuf::pLight = nullptr;

	TransformCbuf::TransformCbuf(Graphics& gfx, UINT slot)
	{
		if (!pVcbuf)
		{
			pVcbuf = std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot);
		}
	}

	void TransformCbuf::Bind(Graphics& gfx) noxnd
	{
		UpdateBindImpl(gfx, GetTransforms(gfx));
	}

	void TransformCbuf::InitializeParentReference(const Drawable& parent) noexcept
	{
		pParent = &parent;
	}

	std::unique_ptr<CloningBindable> TransformCbuf::Clone() const noexcept
	{
		return std::make_unique<TransformCbuf>(*this);
	}

	void TransformCbuf::UpdateBindImpl(Graphics& gfx, const Transforms& tf) noxnd
	{
		assert(pParent != nullptr);
		pVcbuf->Update(gfx, tf);
		pVcbuf->Bind(gfx);
	}

	// --- ZMIE� T� FUNKCJ� ---
	TransformCbuf::Transforms TransformCbuf::GetTransforms(Graphics& gfx) noxnd
	{
		assert(pParent != nullptr);
		assert(pLight != nullptr && "Light pointer in TransformCbuf has not been set!");
		const auto model = pParent->GetTransformXM();
		const auto modelView = model * gfx.GetCamera();
		return {
			DirectX::XMMatrixTranspose(modelView),
			DirectX::XMMatrixTranspose(
				modelView * gfx.GetProjection()
			),
			// --- DODAJ OBLICZENIE ---
			DirectX::XMMatrixTranspose(
				model * pLight->GetViewMatrix() * pLight->GetProjectionMatrix()
			)
		};
	}

	// --- DODAJ T� FUNKCJ� ---
	void TransformCbuf::SetLight(const DirectionalLight* pLight_in)
	{
		pLight = pLight_in;
	}
}
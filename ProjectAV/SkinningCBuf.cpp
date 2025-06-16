#include "SkinningCbuf.h"
#include <algorithm> // for std::min

namespace Bind
{
	// Constructor for original instance
	SkinningCbuf::SkinningCbuf(Graphics& gfx, UINT slot)
		:
		slot(slot)
	{
		pVcbuf = std::make_unique<VertexConstantBuffer<BoneTransforms>>(gfx, slot);
	}

	// Private constructor used by Clone
	SkinningCbuf::SkinningCbuf(UINT slot) noexcept
		:
		slot(slot)
	{
	}

	void SkinningCbuf::Bind(Graphics& gfx) noxnd
	{
		// Lazy-initialize the constant buffer for cloned instances
		if (!pVcbuf)
		{
			pVcbuf = std::make_unique<VertexConstantBuffer<BoneTransforms>>(gfx, slot);
		}

		assert(pParent != nullptr);
		const auto* pBoneTransforms = pParent->GetBoneTransformsPtr();

		if (pBoneTransforms)
		{
			BoneTransforms bt;
			// Copy data from the source vector to the fixed-size array in the cbuffer struct
			const size_t numBones = std::min(BoneTransforms::MAX_BONES, pBoneTransforms->size());
			for (size_t i = 0; i < numBones; ++i)
			{
				// We must transpose the matrices for the shader
				bt.boneTransform[i] = DirectX::XMMatrixTranspose((*pBoneTransforms)[i]);
			}
			// Update the GPU buffer with the new transforms
			pVcbuf->Update(gfx, bt);
			// Bind the buffer to the vertex shader stage of the pipeline
			pVcbuf->Bind(gfx);
		}
	}

	void SkinningCbuf::InitializeParentReference(const Drawable& parent) noexcept
	{
		pParent = &parent;
	}

	std::unique_ptr<CloningBindable> SkinningCbuf::Clone() const noexcept
	{
		// Create a new SkinningCbuf instance. It will not have a pVcbuf or pParent.
		// The pVcbuf will be created on its first Bind call.
		// The pParent will be set when its new owner calls InitializeParentReference.
		return std::make_unique<SkinningCbuf>(slot);
	}
}
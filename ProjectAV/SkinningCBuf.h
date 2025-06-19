#pragma once
#include "Bindable.h"
#include "ConstantBuffers.h"
#include "Drawable.h"
#include <DirectXMath.h>
#include <vector>

namespace Bind
{
	class SkinningCbuf : public CloningBindable
	{
	protected:
		// This struct must exactly match the cbuffer layout in the vertex shader.
		// HLSL's `matrix` is column-major by default, while XMMATRIX is row-major.
		// We will transpose the matrices in C++ before updating the buffer.
		struct BoneTransforms
		{
			static constexpr size_t MAX_BONES = 100;
			DirectX::XMMATRIX boneTransform[MAX_BONES];
		};

	public:
		// slot = 3 corresponds to register(b3) in Skinned_VS.hlsl
		SkinningCbuf(Graphics& gfx, UINT slot = 3u);
		void Bind(Graphics& gfx) noxnd override;
		void InitializeParentReference(const Drawable& parent) noexcept override;
		std::unique_ptr<CloningBindable> Clone() const noexcept override;
		SkinningCbuf(UINT slot) noexcept;

	private:
		// Private constructor for cloning
		

	private:
		// Each instance of SkinningCbuf must have its own unique constant buffer.
		// We use mutable to allow for lazy-initialization in the Bind function.
		mutable std::unique_ptr<VertexConstantBuffer<BoneTransforms>> pVcbuf;
		const Drawable* pParent = nullptr;
		UINT slot;
	};
}
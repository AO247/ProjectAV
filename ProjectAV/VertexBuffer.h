#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"
#include "Vertex.h" // For Dvtx::VertexBuffer, Dvtx::VertexLayout, Dvtx::ConstVertex

namespace Bind
{
	class VertexBuffer : public Bindable
	{
	public:
		VertexBuffer(Graphics& gfx, const std::string& tag, const Dvtx::VertexBuffer& vbuf);
		VertexBuffer(Graphics& gfx, const Dvtx::VertexBuffer& vbuf); // Delegates
		void Bind(Graphics& gfx) noxnd override;

		const Dvtx::VertexLayout& GetLayout() const noexcept; // This can return the layout from cpuDataBuffer or a separate copy

		// **** NEW METHODS TO ACCESS CPU-SIDE DATA ****
		size_t GetVertexCount() const noexcept;
		Dvtx::ConstVertex GetVertex(size_t index) const noexcept;
		// Optional: Direct access to the whole Dvtx::VertexBuffer if needed elsewhere
		// const Dvtx::VertexBuffer& GetCPUDataBuffer() const noexcept;


		static std::shared_ptr<VertexBuffer> Resolve(Graphics& gfx, const std::string& tag,
			const Dvtx::VertexBuffer& vbuf);
		template<typename...Ignore>
		static std::string GenerateUID(const std::string& tag, Ignore&&...ignore)
		{
			return GenerateUID_(tag);
		}
		std::string GetUID() const noexcept override;

	private:
		static std::string GenerateUID_(const std::string& tag);

	protected:
		std::string tag;
		UINT stride;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
		// Dvtx::VertexLayout layout; // Original: we can get this from cpuDataBuffer.GetLayout()
									// Or keep a copy if preferred for directness. Let's keep it for minimal change to GetLayout().
		Dvtx::VertexLayout layout_cache; // Store a copy of the layout for quick access
		Dvtx::VertexBuffer cpuDataBuffer;  // Store the actual CPU-side vertex data
	};
}
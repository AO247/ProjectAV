#pragma once
#include "Bindable.h"
#include <vector> // Required for std::vector
#include <string> // Required for std::string (though already implied by Bindable typically)

namespace Bind
{
	class IndexBuffer : public Bindable
	{
	public:
		IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices);
		IndexBuffer(Graphics& gfx, std::string tag, const std::vector<unsigned short>& indices);
		void Bind(Graphics& gfx) noexcept override;
		UINT GetCount() const noexcept;

		// **** NEW PUBLIC METHODS to access CPU-side index data ****
		unsigned short GetIndex(size_t i) const noexcept;
		// Optional: If you want to provide access to the whole vector
		// const std::vector<unsigned short>& GetCPUIndices() const noexcept;


		static std::shared_ptr<IndexBuffer> Resolve(Graphics& gfx, const std::string& tag,
			const std::vector<unsigned short>& indices);
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
		UINT count;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;

		// **** NEW MEMBER to store CPU-side copy of indices ****
		std::vector<unsigned short> cpuIndices;
	};
}
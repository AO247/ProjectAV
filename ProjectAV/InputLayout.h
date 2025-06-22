#pragma once
#include "Bindable.h"
#include "Vertex.h"

namespace Bind
{
	class VertexShader;

	class InputLayout : public Bindable
	{
	public:
		InputLayout(Graphics& gfx,
			Dvtx::VertexLayout layout,
			const VertexShader& vs);
		InputLayout(Graphics& gfx,
			const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
			const VertexShader& vs);
		void Bind(Graphics& gfx) noxnd override;
		const Dvtx::VertexLayout GetLayout() const noexcept;
		static std::shared_ptr<InputLayout> Resolve(Graphics& gfx,
			const Dvtx::VertexLayout& layout, const VertexShader& vs);
		static std::string GenerateUID(const Dvtx::VertexLayout& layout, const VertexShader& vs);
		static std::shared_ptr<InputLayout> Resolve(Graphics& gfx,
			const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, const VertexShader& vs);
		static std::string GenerateUID(const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, const VertexShader& vs);
		std::string GetUID() const noexcept override;
	protected:
		std::string vertexShaderUID;
		Dvtx::VertexLayout layout;
		std::string rawLayoutCode;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
	};
}
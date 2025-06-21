// In InputLayout.cpp

#include "InputLayout.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"
#include "Vertex.h"
#include "VertexShader.h"
#include <string> // Ensure string is included for std::to_string

namespace Bind
{
    // --- EXISTING CONSTRUCTOR ---
    InputLayout::InputLayout(Graphics& gfx,
        Dvtx::VertexLayout layout_in,
        const VertexShader& vs)
        :
        layout(std::move(layout_in)),
        vertexShaderUID(vs.GetUID()) // Capture shader UID
    {
        INFOMAN(gfx);

        const auto d3dLayout = layout.GetD3DLayout();
        const auto pBytecode = vs.GetBytecode();

        GFX_THROW_INFO(GetDevice(gfx)->CreateInputLayout(
            d3dLayout.data(), (UINT)d3dLayout.size(),
            pBytecode->GetBufferPointer(),
            pBytecode->GetBufferSize(),
            &pInputLayout
        ));
    }
    // --- END EXISTING CONSTRUCTOR ---

    // +++ NEW CONSTRUCTOR IMPLEMENTATION +++
    InputLayout::InputLayout(Graphics& gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& d3dLayout, const VertexShader& vs)
        :
        vertexShaderUID(vs.GetUID())
    {
        INFOMAN(gfx);

        // For this constructor, the Dvtx::VertexLayout member is empty/invalid,
        // so we generate a UID from the raw vector itself.
        rawLayoutCode = GenerateUID(d3dLayout, vs);

        const auto pBytecode = vs.GetBytecode();
        GFX_THROW_INFO(GetDevice(gfx)->CreateInputLayout(
            d3dLayout.data(), (UINT)d3dLayout.size(),
            pBytecode->GetBufferPointer(),
            pBytecode->GetBufferSize(),
            &pInputLayout
        ));
    }


    const Dvtx::VertexLayout InputLayout::GetLayout() const noexcept
    {
        return layout;
    }

    void InputLayout::Bind(Graphics& gfx) noxnd
    {
        INFOMAN_NOHR(gfx);
        GFX_THROW_INFO_ONLY(GetContext(gfx)->IASetInputLayout(pInputLayout.Get()));
    }

    // --- EXISTING RESOLVE ---
    std::shared_ptr<InputLayout> InputLayout::Resolve(Graphics& gfx,
        const Dvtx::VertexLayout& layout, const VertexShader& vs)
    {
        return Codex::Resolve<InputLayout>(gfx, layout, vs);
    }
    // --- END EXISTING RESOLVE ---

    // +++ NEW RESOLVE IMPLEMENTATION +++
    std::shared_ptr<InputLayout> InputLayout::Resolve(Graphics& gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, const VertexShader& vs)
    {
        return Codex::Resolve<InputLayout>(gfx, layout, vs);
    }

    // --- EXISTING GENERATEUID ---
    std::string InputLayout::GenerateUID(const Dvtx::VertexLayout& layout, const VertexShader& vs)
    {
        using namespace std::string_literals;
        return typeid(InputLayout).name() + "#"s + layout.GetCode() + "#"s + vs.GetUID();
    }
    // --- END EXISTING GENERATEUID ---

    // +++ NEW GENERATEUID IMPLEMENTATION +++
    std::string InputLayout::GenerateUID(const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, const VertexShader& vs)
    {
        using namespace std::string_literals;
        std::string raw_code = "";
        for (const auto& d : layout)
        {
            raw_code += d.SemanticName;
            raw_code += std::to_string(d.SemanticIndex);
            raw_code += std::to_string(d.Format);
            raw_code += std::to_string(d.InputSlot);
            raw_code += std::to_string(d.AlignedByteOffset);
            raw_code += std::to_string(d.InputSlotClass);
            raw_code += std::to_string(d.InstanceDataStepRate);
        }
        return typeid(InputLayout).name() + "#"s + raw_code + "#"s + vs.GetUID();
    }

    std::string InputLayout::GetUID() const noexcept
    {
        // If this was created with a raw layout, rawLayoutCode will be filled.
        if (!rawLayoutCode.empty())
        {
            return rawLayoutCode;
        }

        // Otherwise, use the existing Dvtx-based method.
        using namespace std::string_literals;
        return typeid(InputLayout).name() + "#"s + layout.GetCode() + "#"s + vertexShaderUID;
    }

}
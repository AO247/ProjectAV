#include "Mesh.h"
#include "Graphics.h" // Make sure Graphics is included
#include "BindableCommon.h" // For TransformCbuf etc.
#include <sstream> // For exceptions

namespace dx = DirectX;

// --- ModelException Implementation --- (Keep as it was)
ModelException::ModelException(int line, const char* file, std::string note) noexcept
    :
    CException(line, file),
    note(std::move(note))
{
}

const char* ModelException::what() const noexcept
{
    std::ostringstream oss;
    oss << CException::what() << std::endl
        << "[Note] " << GetNote();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* ModelException::GetType() const noexcept
{
    return "Model Exception";
}

const std::string& ModelException::GetNote() const noexcept
{
    return note;
}


// --- Mesh Implementation ---

Mesh::Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bind::Bindable>> bindPtrs)
{
    // Static initialization for Topology (remains the same)
    if (!IsStaticInitialized())
    {
        AddStaticBind(std::make_unique<Bind::Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
    }

    // Add IndexBuffer (remains the same)
    for (auto& pb : bindPtrs)
    {
        if (auto pi = dynamic_cast<Bind::IndexBuffer*>(pb.get()))
        {
            AddIndexBuffer(std::unique_ptr<Bind::IndexBuffer>{ pi });
            pb.release(); // Release ownership as AddIndexBuffer takes it
        }
    }
    // Remove nullptrs from vector after release
    bindPtrs.erase(std::remove(bindPtrs.begin(), bindPtrs.end(), nullptr), bindPtrs.end());


    // Add other bindables (remains the same)
    for (auto& pb : bindPtrs)
    {
        AddBind(std::move(pb));
    }

    // Add Transform Constant Buffer - THIS IS IMPORTANT
    // It will use the GetTransformXM() which now returns the per-draw transform
    AddBind(std::make_unique<Bind::TransformCbuf>(gfx, *this));

    // Initialize meshTransform to identity
    dx::XMStoreFloat4x4(&meshTransform, dx::XMMatrixIdentity());
}

// Draw method now takes the transform
void Mesh::Draw(Graphics& gfx, DirectX::FXMMATRIX transform) const noxnd
{
    // Store the passed transform so GetTransformXM() can retrieve it for the cbuffer
    dx::XMStoreFloat4x4(&meshTransform, transform);
    Drawable::Draw(gfx); // Call the base class Draw which binds everything and calls DX context DrawIndexed
}

// GetTransformXM now returns the transform set during the Draw call
DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
    return dx::XMLoadFloat4x4(&meshTransform);
}
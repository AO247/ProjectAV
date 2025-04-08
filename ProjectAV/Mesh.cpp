#include "Mesh.h"
#include "Graphics.h" 
#include "BindableCommon.h" // Include all necessary bindable headers
#include <sstream> 
#include <stdexcept> 

namespace dx = DirectX;

// --- ModelException Implementation --- 
// (Ensure definitions are present here)
ModelException::ModelException(int line, const char* file, std::string note) noexcept
    :
    CException(line, file), // Make sure CException base constructor matches
    note(std::move(note))
{
}

const char* ModelException::what() const noexcept
{
    // If CException has its own whatBuffer, use it, otherwise manage locally
    std::ostringstream oss;
    // Assuming CException::what() provides base info
    oss << CException::what() << std::endl
        << "[Note] " << GetNote();
    // Need a way to store the buffer. If CException has whatBuffer, reuse it.
    // If not, add one here: static thread_local std::string localWhatBuffer;
    // For now, assuming CException manages its buffer:
    whatBuffer = oss.str(); // Assign to base class's buffer if it exists & is accessible
    return whatBuffer.c_str(); // Return pointer to base class's buffer
    // If CException doesn't provide whatBuffer:
    // static thread_local std::string localWhatBuffer; // Or make it a member of ModelException
    // localWhatBuffer = oss.str();
    // return localWhatBuffer.c_str();
}

const char* ModelException::GetType() const noexcept
{
    return "Model Exception"; // Or "Project AV Model Exception" etc.
}

const std::string& ModelException::GetNote() const noexcept
{
    return note;
}


// --- Mesh Implementation ---

Mesh::Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs)
{
    // Add default topology using Resolve (requires Topology.h via BindableCommon.h)
    AddBind(Bind::Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

    // Add all bindables passed in the vector
    for (auto& pBind : bindPtrs)
    {
        if (pBind) // Add a null check just in case
        {
            AddBind(std::move(pBind)); // AddBind takes shared_ptr now
        }
    }

    // Add the Transform Constant Buffer - *not* using Resolve, as it's unique per Drawable
    // Ensure TransformCbuf constructor is compatible (takes Graphics&, const Drawable&)
    // Requires TransformCbuf.h via BindableCommon.h
    AddBind(std::make_shared<Bind::TransformCbuf>(gfx, *this));

    // Initialize meshTransform to identity
    dx::XMStoreFloat4x4(&meshTransform, dx::XMMatrixIdentity());
}

// Set the transform to be used by GetTransformXM during the next Draw call
void Mesh::SetTransform(DirectX::FXMMATRIX transform) const noxnd
{
    // Store transposed matrix if TransformCbuf expects it, otherwise store directly.
    // Assuming TransformCbuf handles potential transposition internally based on shader needs.
    dx::XMStoreFloat4x4(&meshTransform, transform);
}

// GetTransformXM returns the transform set by SetTransform
DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
    // This matrix should be the world matrix ready for the VS Constant Buffer
    return dx::XMLoadFloat4x4(&meshTransform);
}
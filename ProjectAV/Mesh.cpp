#include "Mesh.h"
#include "Graphics.h" 
#include "Material.h" // Include the new Material class
#include "FrameCommander.h" // Include for Submit
#include <assimp/mesh.h> // For aiMesh definition
#include <sstream> 

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

Mesh::Mesh(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale) noxnd
    : Drawable(gfx, mat, mesh, scale) // Call base Drawable constructor
{
    // Drawable constructor now handles pVertices, pIndices, pTopology, and techniques
    dx::XMStoreFloat4x4(&transform, dx::XMMatrixIdentity());
}

// **** NEW SUBMIT METHOD ****
void Mesh::Submit(FrameCommander& frame, dx::FXMMATRIX accumulatedTransform) const noxnd
{
    dx::XMStoreFloat4x4(&transform, accumulatedTransform); // Store the final world transform for this mesh
    Drawable::Submit(frame); // Call base Drawable's Submit, which iterates techniques
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
    // This is called by TransformCbuf within a Technique's Step
    return DirectX::XMLoadFloat4x4(&transform);
}
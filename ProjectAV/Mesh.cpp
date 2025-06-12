#include "Mesh.h"
#include "Graphics.h" 
#include "Material.h" 
#include <assimp/mesh.h> 
#include <sstream> 

namespace dx = DirectX;

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




Mesh::Mesh(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale) noxnd
    : Drawable(gfx, mat, mesh, scale) 
{

    dx::XMStoreFloat4x4(&transform, dx::XMMatrixIdentity());
}


void Mesh::Submit(dx::FXMMATRIX accumulatedTransform) const noxnd
{
    dx::XMStoreFloat4x4(&transform, accumulatedTransform); 
    Drawable::Submit(); 
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
    
    return DirectX::XMLoadFloat4x4(&transform);
}
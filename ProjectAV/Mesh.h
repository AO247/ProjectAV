#pragma once
#include "Drawable.h" // Your new Drawable base
#include "CException.h" 
#include <vector>
#include <memory>
#include <string>
#include <DirectXMath.h>
#include "ConditionalNoexcept.h"
// Forward declarations
class Graphics;
class Material; // Forward declare the new Material class
struct aiMesh;   // Forward declare Assimp mesh
class FrameCommander; // Forward declare for Submit
class Node;

class ModelException : public CException
{
public:
    ModelException(int line, const char* file, std::string note) noexcept;
    const char* what() const noexcept override;
    const char* GetType() const noexcept override;
    const std::string& GetNote() const noexcept;
private:
    std::string note;
};


// --- Mesh Declaration ---
// Inherit from the new Drawable base class
class Mesh : public Drawable
{
public:
    // **** MODIFIED CONSTRUCTOR ****
    Mesh(
        Node* pOwner,
        Graphics& gfx,
        std::shared_ptr<Bind::VertexBuffer> pVB,
        std::shared_ptr<Bind::IndexBuffer> pIB,
        const Material& mat // To pass to Drawable for techniques
    ) noxnd;
    DirectX::XMMATRIX GetTransformXM() const noexcept override;

    // **** NEW SUBMIT METHOD ****
    void Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulatedTransform) const noxnd;

private:
    mutable DirectX::XMFLOAT4X4 transform = {}; // For storing world transform for this mesh
};
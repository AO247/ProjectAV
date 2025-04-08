#pragma once
#include "Drawable.h" // Include the new base class
#include "CException.h" // Assuming CException is your base exception class
#include <vector>
#include <memory>           // For shared_ptr
#include <string>
#include <DirectXMath.h>
#include "ConditionalNoexcept.h"

// Forward declare Graphics and Bindable
class Graphics;
namespace Bind { class Bindable; }

// --- ModelException Declaration ---
// (Moved definition to Mesh.cpp)
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
    // Constructor now takes shared_ptrs
    Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs);

    // GetTransformXM is required by the Drawable base class
    DirectX::XMMATRIX GetTransformXM() const noexcept override;

    // Method to set the transform before drawing (called by ModelInternalNode)
    void SetTransform(DirectX::FXMMATRIX transform) const noxnd;

private:
    // Store the transform passed via SetTransform for GetTransformXM to use
    mutable DirectX::XMFLOAT4X4 meshTransform = {};
};
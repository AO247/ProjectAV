#pragma once
#include "DrawableBase.h"
#include "BindableCommon.h"
#include "Vertex.h"
#include <vector>
#include <memory>
#include <DirectXMath.h>
#include "ConditionalNoexcept.h"

// Forward declare Graphics if necessary, or include Graphics.h
class Graphics;

class Mesh : public DrawableBase<Mesh>
{
public:
    // Constructor takes ownership of bindables
    Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bind::Bindable>> bindPtrs);

    // Draw with a specific world transform
    void Draw(Graphics& gfx, DirectX::FXMMATRIX transform) const noxnd;

    // Override GetTransformXM to return the transform passed during Draw
    DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
    // Store the transform passed during the Draw call for the TransformCbuf
    mutable DirectX::XMFLOAT4X4 meshTransform;
};

// Exception class remains the same - can stay here or move to a common exception file
class ModelException : public CException
{
    // ... (keep existing ModelException code) ...
public:
    ModelException(int line, const char* file, std::string note) noexcept;
    const char* what() const noexcept override;
    const char* GetType() const noexcept override;
    const std::string& GetNote() const noexcept;
private:
    std::string note;
};
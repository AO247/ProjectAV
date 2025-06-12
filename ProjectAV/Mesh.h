#pragma once
#include "Drawable.h"
#include "CException.h" 
#include <vector>
#include <memory>
#include <string>
#include <DirectXMath.h>
#include "ConditionalNoexcept.h"

class Graphics;
class Material; 
struct aiMesh;  


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

class Mesh : public Drawable
{
public:

    Mesh(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale = 1.0f) noxnd;

    DirectX::XMMATRIX GetTransformXM() const noexcept override;


    void Submit(DirectX::FXMMATRIX accumulatedTransform) const noxnd;

private:
    mutable DirectX::XMFLOAT4X4 transform = {};
};
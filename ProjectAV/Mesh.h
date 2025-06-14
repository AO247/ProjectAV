#pragma once
#include "Drawable.h"
#include <vector>
#include <memory>
#include <string>
#include <DirectXMath.h>

class Graphics;
class Material;
struct aiMesh;

class Mesh : public Drawable
{
public:
    // Modified constructor
    Mesh(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale = 1.0f);

    DirectX::XMMATRIX GetTransformXM() const noexcept override;
    void Submit(DirectX::FXMMATRIX accumulatedTransform) const noxnd;

private:
    mutable DirectX::XMFLOAT4X4 transform = {};
};
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
    Mesh(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale = 1.0f, bool castsShadow = true);
    void LinkTechniques(Rgph::RenderGraph& rg) override;
    DirectX::XMMATRIX GetTransformXM() const noexcept override;
    void Submit(DirectX::FXMMATRIX accumulatedTransform) const noxnd;
    void Submit(DirectX::FXMMATRIX accumulatedTransform, const std::vector<DirectX::XMMATRIX>* pBoneTransforms) const noxnd;
    const std::vector<DirectX::XMMATRIX>* GetBoneTransformsPtr() const noexcept override;


    bool DoesCastShadow() const { return m_castsShadow; }

private:
    mutable DirectX::XMFLOAT4X4 transform = {};
    mutable std::vector<DirectX::XMMATRIX> boneTransforms;
    bool m_castsShadow;

};
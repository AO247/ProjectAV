#pragma once
#include "Component.h"
#include <vector>
#include <memory>
#include <string>
#include <DirectXMath.h>
#include "ConditionalNoexcept.h"
#include <SimpleMath.h>
#include "Technique.h"
#include <map>
#include <assimp/scene.h>

class Mesh;
class Graphics;
class FrameCommander;
namespace Rgph
{
    class RenderGraph;
}

struct XMFLOAT3Less {
    bool operator()(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b) const {
        if (a.x != b.x) return a.x < b.x;
        if (a.y != b.y) return a.y < b.y;
        return a.z < b.z;
    }
};

class ModelComponent : public Component
{
public:
    //================================================================================
    // NOWA STRUKTURA INFORMACJI O KOŒCIACH
    //================================================================================
    struct BoneInfo
    {
        int id;
        DirectX::XMMATRIX offset;
    };

    struct Triangle
    {
        DirectX::SimpleMath::Vector3 v0;
        DirectX::SimpleMath::Vector3 v1;
        DirectX::SimpleMath::Vector3 v2;

        Triangle(const DirectX::SimpleMath::Vector3& p0,
            const DirectX::SimpleMath::Vector3& p1,
            const DirectX::SimpleMath::Vector3& p2)
            : v0(p0), v1(p1), v2(p2) {
        }
    };

    ModelComponent(Node* owner, Graphics& gfx, const std::string& modelFile, float scale = 1.0f, bool isSkinned = false, bool castsShadowByDefault = true);
    virtual ~ModelComponent() = default;

    void Submit(Graphics& gfx, DirectX::FXMMATRIX worldTransform) const noxnd;
    void LinkTechniques(Rgph::RenderGraph&);

    std::vector<DirectX::SimpleMath::Vector3> GetAllUniqueVertices() const;
    std::vector<ModelComponent::Triangle> GetAllTriangles() const;

    std::vector<std::unique_ptr<Mesh>> meshPtrs;
    void AddTechnique(Technique technique);
    std::vector<Technique> techniques;

    //================================================================================
    // MAPA INFORMACJI O KOŒCIACH I LICZNIK
    //================================================================================
    std::map<std::string, BoneInfo>& GetBoneInfoMap() { return m_BoneInfoMap; }
    int& GetBoneCount() { return m_BoneCounter; }


    const bool skinnedCharacter;

private:
    void ExtractBoneInfo(const aiScene& scene);

    std::map<std::string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;
};
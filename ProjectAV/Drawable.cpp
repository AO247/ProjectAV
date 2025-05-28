#include "Drawable.h"
#include "GraphicsThrowMacros.h"
#include "BindableCommon.h"
#include "BindableCodex.h"
#include <assimp/scene.h>
#include "Material.h"

using namespace Bind;

#include "Drawable.h"
#include "FrameCommander.h" // For FrameCommander
#include "Node.h"           // For Node and GetComponent
#include "AnimationComponent.h" // For AnimationComponent
#include "VertexBuffer.h"   // For Bind::VertexBuffer and GetLayout()
#include "Vertex.h"         // For Dvtx::VertexLayout and Dvtx::VertexLayout::BoneIDs
#include <sstream>          // For std::ostringstream (optional for logging)

void Drawable::Submit(FrameCommander& frame) const noexcept
{


    bool useSkinnedPath = false;
    AnimationComponent* animComp = pOwnerNode->GetComponent<AnimationComponent>();


    if (!pOwnerNode) {
        // OutputDebugStringA("Drawable::Submit - ERROR: pOwnerNode is null!\n"); // Using ostringstream below
        std::ostringstream oss; oss << "Drawable::Submit - ERROR: pOwnerNode is null for a Drawable being submitted!\n";
        OutputDebugStringA(oss.str().c_str());
        return;
    }
    if (!pVertices) {
        std::ostringstream oss; oss << "Drawable::Submit - ERROR: pVertices (VertexBuffer) is null for Drawable owned by Node: " << pOwnerNode->GetName() << "\n";
        OutputDebugStringA(oss.str().c_str());
        return;
    }

    // Determine if we should use the skinned rendering path
    if (pVertices->GetLayout().Has(Dvtx::VertexLayout::BoneIDs)) { // Condition 1: Vertex data has bones
        //if (animComp) { // Condition 2: AnimationComponent exists
        //    // If animComp exists, we use the skinned path.
        //    // If it's playing, it animates. If not playing, Update should set bind pose.
        //    useSkinnedPath = true;

        //}
        //else {
        //    // Has bone data in VB, but no AnimationComponent. This is unusual.
        //    // Render static; bone data will be unused by static shader.
        //    std::ostringstream oss;
        //    oss << "Drawable::Submit Warning: Drawable for Node '" << pOwnerNode->GetName()
        //        << "' has bone data in VertexBuffer, but no AnimationComponent. Rendering static.\n";
        //    OutputDebugStringA(oss.str().c_str());

        //    useSkinnedPath = false;
        //}

        useSkinnedPath = true;
    }

    bool primaryTechniqueSubmitted = false;
    std::string selectedTechniqueName = "None";

    for (const auto& tech : techniques) {
        const std::string& techName = tech.GetName();
        bool submitThisTech = false;

        if (useSkinnedPath) {
            // If your conditionally modified "Phong" is named "PhongSkinned_char" or similar:
            if (techName == "Skinned" || techName == "PhongSkinned_char") {
                submitThisTech = true;
                selectedTechniqueName = techName;
            }
        }
        else { // Use static path
            if (techName == "Phong") {
                submitThisTech = true;
                selectedTechniqueName = techName;
            }
        }

        if (submitThisTech) {
            tech.Submit(frame, *this);
            primaryTechniqueSubmitted = true;
            // If you only want one primary technique (Phong or Skinned), you can break here.
            // break; // Uncomment if "Phong" and "Skinned" are mutually exclusive for a draw.
        }

        // Handle Outline or other general techniques that might always run
        // or have their own conditions.
        if (techName == "Outline") {
            // tech.Submit(frame, *this); // Submit outline if needed
        }
    }

    if (!primaryTechniqueSubmitted && !techniques.empty()) {
        std::ostringstream oss;
        oss << "Drawable::Submit Warning: No suitable primary technique ('"
            << (useSkinnedPath ? "Skinned' or 'PhongSkinned_char" : "Phong")
            << "') found for drawable on Node: " << pOwnerNode->GetName()
            << ". VB Has BoneIDs: " << pVertices->GetLayout().Has(Dvtx::VertexLayout::BoneIDs)
            << ", AnimComp exists: " << (animComp != nullptr)
            << (animComp ? (animComp->IsPlaying() ? ", Anim IsPlaying" : ", Anim NotPlaying") : "")
            << ". Available techniques: ";
        for (const auto& tech : techniques) oss << tech.GetName() << " ";
        oss << "\n";
        OutputDebugStringA(oss.str().c_str());
        // As a fallback, maybe submit the first technique? Or render nothing for this object.
        // techniques.front().Submit(frame, *this);
    }
}

Drawable::Drawable(
    Node* pOwnerNode,
    Graphics& gfx,
    std::shared_ptr<Bind::VertexBuffer> pVB,
    std::shared_ptr<Bind::IndexBuffer> pIB,
    const Material& mat // Make sure Material::GetTechniques() is const or mat is non-const
) noexcept
    : pOwnerNode(pOwnerNode), pVertices(std::move(pVB)), pIndices(std::move(pIB)) // Initialize members with passed-in smart pointers
{
    pTopology = Bind::Topology::Resolve(gfx); // This can remain

    // Handle techniques - ensure mat.GetTechniques() is callable and types match
    // This assumes Material::GetTechniques() returns something iterable (like std::vector<Technique>)
    // and that Technique is movable.
    // If Material::GetTechniques() returns by const ref, you might need to copy.
    const auto& matTechniques = mat.GetTechniques(); // Assuming: const std::vector<Technique>& GetTechniques() const;
    for (const auto& const_tech_ref : matTechniques)
    {
        Technique tech_copy = const_tech_ref; // Make a copy because AddTechnique takes by value and moves
        AddTechnique(std::move(tech_copy));
    }
    // Or, if AddTechnique took by const ref and copied internally, or if Technique was cheap to copy:
    // for (const auto& tech_to_add : mat.GetTechniques()) {
    //    AddTechnique(tech_to_add);
    // }
}

Node* Drawable::GetParentNode() const noexcept // ***** NEW *****
{
    return pOwnerNode;
}

void Drawable::AddTechnique( Technique tech_in ) noexcept
{
	tech_in.InitializeParentReferences( *this );
	techniques.push_back( std::move( tech_in ) );
}

void Drawable::Bind( Graphics& gfx ) const noexcept
{
	pTopology->Bind( gfx );
	pIndices->Bind( gfx );
	pVertices->Bind( gfx );
}

void Drawable::Accept( TechniqueProbe& probe )
{
	for( auto& t : techniques )
	{
		t.Accept( probe );
	}
}

UINT Drawable::GetIndexCount() const noxnd
{
	return pIndices->GetCount();
}

Drawable::~Drawable()
{}
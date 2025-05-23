#include "Drawable.h"
#include "GraphicsThrowMacros.h"
#include "BindableCommon.h"
#include "BindableCodex.h"
#include <assimp/scene.h>
#include "Material.h"

using namespace Bind;


void Drawable::Submit( FrameCommander& frame ) const noexcept
{
	for( const auto& tech : techniques )
	{
		tech.Submit( frame,*this );
	}
}

Drawable::Drawable(
    Graphics& gfx,
    std::shared_ptr<Bind::VertexBuffer> pVB,
    std::shared_ptr<Bind::IndexBuffer> pIB,
    const Material& mat // Make sure Material::GetTechniques() is const or mat is non-const
) noexcept
    : pVertices(std::move(pVB)), pIndices(std::move(pIB)) // Initialize members with passed-in smart pointers
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
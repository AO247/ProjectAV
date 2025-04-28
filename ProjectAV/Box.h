#pragma once
#include <optional>
#include "Vertex.h"           // Assuming this defines Dvtx::VertexLayout and Dvtx::VertexBuffer
#include "IndexedTriangleList.h" // Assuming this defines IndexedTriangleList
#include <DirectXMath.h>
#include <vector>
#include <array>              // Useful for defining vertices concisely

class Box
{
public:
    // Generates vertices and indices for an AABB defined by min/max extents
    static IndexedTriangleList Make(Dvtx::VertexLayout layout, DirectX::XMFLOAT3 minExtents, DirectX::XMFLOAT3 maxExtents)
    {
        namespace dx = DirectX;

        // Define the 8 vertices of the AABB in the same order as Cube::Make implicitly uses
        // relative to the center (even though we use min/max here)
        // This order allows using Cube's indices directly.
        std::vector<DirectX::XMFLOAT3> vertices(8);
        vertices[0] = { minExtents.x, minExtents.y, minExtents.z }; // Cube 0: -side, -side, -side
        vertices[1] = { maxExtents.x, minExtents.y, minExtents.z }; // Cube 1:  side, -side, -side
        vertices[2] = { minExtents.x, maxExtents.y, minExtents.z }; // Cube 2: -side,  side, -side
        vertices[3] = { maxExtents.x, maxExtents.y, minExtents.z }; // Cube 3:  side,  side, -side
        vertices[4] = { minExtents.x, minExtents.y, maxExtents.z }; // Cube 4: -side, -side,  side
        vertices[5] = { maxExtents.x, minExtents.y, maxExtents.z }; // Cube 5:  side, -side,  side
        vertices[6] = { minExtents.x, maxExtents.y, maxExtents.z }; // Cube 6: -side,  side,  side
        vertices[7] = { maxExtents.x, maxExtents.y, maxExtents.z }; // Cube 7:  side,  side,  side

        // Create the vertex buffer using the provided layout
        Dvtx::VertexBuffer vb{ std::move(layout) };
        for (const auto& cornerPos : vertices)
        {
            // EmplaceBack needs to match the layout.
            // Assuming layout only requires Position3D for this basic box.
            // If layout needs more (e.g., normals), this needs adjustment.
            vb.EmplaceBack(cornerPos);
        }

        // --- Use the exact same indices as Cube::Make ---
        // These indices assume Clockwise (CW) winding order for front faces
        // relative to the outside view, matching DirectX default culling.
        std::vector<unsigned short> indices = {
            0,2,1, 2,3,1, // Back Face   (-Z) triangles (0,2,1), (2,3,1)
            1,3,5, 3,7,5, // Right Face  (+X) triangles (1,3,5), (3,7,5)
            2,6,3, 3,6,7, // Top Face    (+Y) triangles (2,6,3), (3,6,7)
            4,5,7, 4,7,6, // Front Face  (+Z) triangles (4,5,7), (4,7,6)
            0,4,2, 2,4,6, // Left Face   (-X) triangles (0,4,2), (2,4,6)
            0,1,4, 1,5,4  // Bottom Face (-Y) triangles (0,1,4), (1,5,4)
        };
        // --- End index change ---

        return { std::move(vb),std::move(indices) };
    }

    // Convenience overload remains the same
    static IndexedTriangleList Make(DirectX::XMFLOAT3 minExtents, DirectX::XMFLOAT3 maxExtents, std::optional<Dvtx::VertexLayout> layout = std::nullopt)
    {
        using Element = Dvtx::VertexLayout::ElementType;
        if (!layout)
        {
            layout = Dvtx::VertexLayout{}.Append(Element::Position3D);
        }
        return Make(std::move(*layout), minExtents, maxExtents);
    }
};
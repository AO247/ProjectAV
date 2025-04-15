#pragma once
#include <optional>
#include "Vertex.h"           // Assuming this defines Dvtx::VertexLayout and Dvtx::VertexBuffer
#include "IndexedTriangleList.h" // Assuming this defines IndexedTriangleList
#include <DirectXMath.h>
#include <vector>
#include <array>              // Useful for defining vertices concisely

// Note: Including CMath.h seems unnecessary based on usage, 
// unless it defines PI or other constants used elsewhere.
// DirectX::XM_PI can be used if needed.

class Box
{
public:
    // Generates vertices and indices for an AABB defined by min/max extents
    static IndexedTriangleList Make(Dvtx::VertexLayout layout, DirectX::XMFLOAT3 minExtents, DirectX::XMFLOAT3 maxExtents)
    {
        namespace dx = DirectX;

        // Define the 8 vertices of the AABB
        const std::array<dx::XMFLOAT3, 8> corners = {
            dx::XMFLOAT3{ minExtents.x, minExtents.y, minExtents.z }, // 0: Bottom-Left-Back
            dx::XMFLOAT3{ maxExtents.x, minExtents.y, minExtents.z }, // 1: Bottom-Right-Back
            dx::XMFLOAT3{ minExtents.x, maxExtents.y, minExtents.z }, // 2: Top-Left-Back
            dx::XMFLOAT3{ maxExtents.x, maxExtents.y, minExtents.z }, // 3: Top-Right-Back
            dx::XMFLOAT3{ minExtents.x, minExtents.y, maxExtents.z }, // 4: Bottom-Left-Front
            dx::XMFLOAT3{ maxExtents.x, minExtents.y, maxExtents.z }, // 5: Bottom-Right-Front
            dx::XMFLOAT3{ minExtents.x, maxExtents.y, maxExtents.z }, // 6: Top-Left-Front
            dx::XMFLOAT3{ maxExtents.x, maxExtents.y, maxExtents.z }  // 7: Top-Right-Front
        };

        // Create the vertex buffer
        Dvtx::VertexBuffer vb{ std::move(layout) };
        for (const auto& corner : corners)
        {
            // EmplaceBack assumes it knows how to handle the layout
            // (e.g., only fills Position3D if that's what the layout specifies)
            // If other attributes (normals, texcoords) are needed, they must be calculated
            // and passed appropriately based on the layout's requirements.
            // For a simple solid box, only position might be needed initially.
            vb.EmplaceBack(corner);
        }

        // Define the indices for the 12 triangles (2 per face)
        // Winding order is generally counter-clockwise for front-facing faces in DirectX
        std::vector<unsigned short> indices = {
            // Front face (+Z)
            4, 6, 7,  4, 7, 5,
            // Back face (-Z)
            1, 3, 2,  1, 2, 0,
            // Left face (-X)
            0, 2, 6,  0, 6, 4,
            // Right face (+X)
            5, 7, 3,  5, 3, 1,
            // Top face (+Y)
            2, 3, 7,  2, 7, 6,
            // Bottom face (-Y)
            4, 5, 1,  4, 1, 0
        };


        return { std::move(vb),std::move(indices) };
    }

    // Convenience overload with optional layout (defaults to Position3D)
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
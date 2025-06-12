#pragma once
#include <optional>
#include "Vertex.h"           
#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include <vector>
#include <array>              

class Box
{
public:
    static IndexedTriangleList Make(Dvtx::VertexLayout layout, DirectX::XMFLOAT3 minExtents, DirectX::XMFLOAT3 maxExtents)
    {
        namespace dx = DirectX;

        std::vector<DirectX::XMFLOAT3> vertices(8);
        vertices[0] = { minExtents.x, minExtents.y, minExtents.z }; 
        vertices[1] = { maxExtents.x, minExtents.y, minExtents.z }; 
        vertices[2] = { minExtents.x, maxExtents.y, minExtents.z }; 
        vertices[3] = { maxExtents.x, maxExtents.y, minExtents.z }; 
        vertices[4] = { minExtents.x, minExtents.y, maxExtents.z }; 
        vertices[5] = { maxExtents.x, minExtents.y, maxExtents.z }; 
        vertices[6] = { minExtents.x, maxExtents.y, maxExtents.z }; 
        vertices[7] = { maxExtents.x, maxExtents.y, maxExtents.z }; 

        Dvtx::VertexBuffer vb{ std::move(layout) };
        for (const auto& cornerPos : vertices)
        {
            vb.EmplaceBack(cornerPos);
        }

        std::vector<unsigned short> indices = {
            0,2,1, 2,3,1, 
            1,3,5, 3,7,5, 
            2,6,3, 3,6,7, 
            4,5,7, 4,7,6, 
            0,4,2, 2,4,6, 
            0,1,4, 1,5,4  
        };

        return { std::move(vb),std::move(indices) };
    }

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
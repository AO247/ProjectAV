#pragma once
#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include "Vertex.h"

class Box
{
public:
	static IndexedTriangleList Make(DirectX::XMFLOAT3 minExtents, DirectX::XMFLOAT3 maxExtents)
	{
		Dvtx::VertexLayout layout = Dvtx::VertexLayout{}.Append(Dvtx::VertexLayout::ElementType::Position3D);
		Dvtx::VertexBuffer vb{ std::move(layout) };

		float xEdgeSize = abs(minExtents.x - maxExtents.x);
		float yEdgeSize = abs(minExtents.y - maxExtents.y);
		float zEdgeSize = abs(minExtents.z - maxExtents.z);

		// minExtents = bottom left corner closer to camera
		// maxExtents = upper right corner away from camera

		std::vector<unsigned short> indices;

		// front wall
		vb.EmplaceBack(minExtents);
		vb.EmplaceBack(DirectX::XMFLOAT3(minExtents.x + xEdgeSize,
										 minExtents.y, 
										 minExtents.z));
		vb.EmplaceBack(DirectX::XMFLOAT3(minExtents.x,
										 minExtents.y + yEdgeSize,
										 minExtents.z));
		vb.EmplaceBack(DirectX::XMFLOAT3(minExtents.x + xEdgeSize,
										 minExtents.y + yEdgeSize,
										 minExtents.z));
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(2);
		indices.push_back(3);
		indices.push_back(1);

		// left wall
		vb.EmplaceBack(minExtents);
		vb.EmplaceBack(DirectX::XMFLOAT3(minExtents.x,
										 minExtents.y,
										 minExtents.z + zEdgeSize));
		vb.EmplaceBack(DirectX::XMFLOAT3(minExtents.x,
										 minExtents.y + yEdgeSize,
										 minExtents.z));
		vb.EmplaceBack(DirectX::XMFLOAT3(minExtents.x,
										 minExtents.y + yEdgeSize,
										 minExtents.z + zEdgeSize));

		indices.push_back(4);
		indices.push_back(5);
		indices.push_back(6);
		indices.push_back(6);
		indices.push_back(7);
		indices.push_back(5);

		// right wall
		vb.EmplaceBack(DirectX::XMFLOAT3(minExtents.x + xEdgeSize,
										 minExtents.y,
										 minExtents.z));
		vb.EmplaceBack(DirectX::XMFLOAT3(minExtents.x + xEdgeSize,
										 minExtents.y + yEdgeSize,
										 minExtents.z));
		vb.EmplaceBack(DirectX::XMFLOAT3(minExtents.x + xEdgeSize,
										 minExtents.y,
										 minExtents.z + zEdgeSize));
		vb.EmplaceBack(DirectX::XMFLOAT3(minExtents.x + xEdgeSize,
										 minExtents.y + yEdgeSize,
										 minExtents.z + zEdgeSize));

		indices.push_back(8);
		indices.push_back(9);
		indices.push_back(10);
		indices.push_back(9);
		indices.push_back(11);
		indices.push_back(10);

		// back wall
		vb.EmplaceBack(DirectX::XMFLOAT3(minExtents.x + xEdgeSize,
										 minExtents.y,
										 minExtents.z + zEdgeSize));
		vb.EmplaceBack(DirectX::XMFLOAT3(minExtents.x + xEdgeSize,
										 minExtents.y + yEdgeSize,
										 minExtents.z + zEdgeSize));
		vb.EmplaceBack(DirectX::XMFLOAT3(minExtents.x,
										 minExtents.y,
										 minExtents.z + zEdgeSize));
		vb.EmplaceBack(DirectX::XMFLOAT3(minExtents.x,
										 minExtents.y + yEdgeSize,
										 minExtents.z + zEdgeSize));

		indices.push_back(12);
		indices.push_back(13);
		indices.push_back(14);
		indices.push_back(14);
		indices.push_back(15);
		indices.push_back(13);

		return { std::move(vb),std::move(indices) };
	}
};


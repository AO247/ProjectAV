#pragma once

#include <optional>
#include "Vertex.h"           // For Dvtx::* types
#include "IndexedTriangleList.h" // For return type
#include <DirectXMath.h>
#include <vector>
#include "CMath.h"          // For PI definition

class Capsule
{
public:
    // Generates vertices and indices for a capsule 
    // defined by base point, tip point, radius, and tessellation level.
    static IndexedTriangleList MakeTesselated(Dvtx::VertexLayout layout,
        DirectX::XMFLOAT3 base,
        DirectX::XMFLOAT3 tip,
        float radius,
        int latDiv,  // Divisions for the hemispheres
        int longDiv) // Divisions around the cylinder/hemispheres
    {
        namespace dx = DirectX;
        assert(latDiv >= 3);
        assert(longDiv >= 3);
        assert(radius > 0.0f);

        Dvtx::VertexBuffer vb{ std::move(layout) };
        std::vector<unsigned short> indices;

        // Calculate orientation and segment vectors
        dx::XMVECTOR baseVec = dx::XMLoadFloat3(&base);
        dx::XMVECTOR tipVec = dx::XMLoadFloat3(&tip);
        dx::XMVECTOR axisVec = dx::XMVectorSubtract(tipVec, baseVec);
        dx::XMVECTOR axisNormVec = dx::XMVector3Normalize(axisVec);

        // --- Find perpendicular axes for generating rings/caps ---
        dx::XMVECTOR perpX, perpY;
        // Create a vector not parallel to the axis
        dx::XMVECTOR nonParallelVec = (abs(dx::XMVectorGetY(axisNormVec)) < 0.99f) ?
            dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) :
            dx::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
        // Calculate perpendicular vectors using cross products
        perpX = dx::XMVector3Normalize(dx::XMVector3Cross(nonParallelVec, axisNormVec));
        perpY = dx::XMVector3Normalize(dx::XMVector3Cross(axisNormVec, perpX));


        // --- Generate Vertices ---

        // -- Cylinder Rings --
        const float longitudeAngle = 2.0f * PI / longDiv;
        unsigned short baseRingStartIdx = (unsigned short)vb.Size(); // Index of first base ring vertex
        for (int iLong = 0; iLong < longDiv; ++iLong)
        {
            float angle = longitudeAngle * iLong;
            dx::XMVECTOR ringOffset = dx::XMVectorAdd(dx::XMVectorScale(perpX, cosf(angle)), dx::XMVectorScale(perpY, sinf(angle)));
            ringOffset = dx::XMVectorScale(ringOffset, radius);

            // Add base ring vertex
            dx::XMFLOAT3 baseRingPos;
            dx::XMStoreFloat3(&baseRingPos, dx::XMVectorAdd(baseVec, ringOffset));
            vb.EmplaceBack(baseRingPos);
        }
        unsigned short topRingStartIdx = (unsigned short)vb.Size(); // Index of first top ring vertex
        for (int iLong = 0; iLong < longDiv; ++iLong)
        {
            float angle = longitudeAngle * iLong;
            dx::XMVECTOR ringOffset = dx::XMVectorAdd(dx::XMVectorScale(perpX, cosf(angle)), dx::XMVectorScale(perpY, sinf(angle)));
            ringOffset = dx::XMVectorScale(ringOffset, radius);

            // Add top ring vertex
            dx::XMFLOAT3 topRingPos;
            dx::XMStoreFloat3(&topRingPos, dx::XMVectorAdd(tipVec, ringOffset));
            vb.EmplaceBack(topRingPos);
        }

        // -- Hemisphere Caps --
        const float latitudeAngle = PI / 2.0f / latDiv; // Angle step for hemisphere latitude

        // Bottom Hemisphere (excluding the base ring itself)
        unsigned short bottomCapStartIdx = (unsigned short)vb.Size();
        for (int iLat = 1; iLat < latDiv; ++iLat) // Start from 1 to avoid duplicating base ring
        {
            // Calculate latitude angle relative to the base plane normal (-axisNormVec)
            float latAngle = latitudeAngle * iLat;

            // Calculate point along the radius vector for this latitude
            dx::XMVECTOR latRadiusVec = dx::XMVectorScale(axisNormVec, -sinf(latAngle)); // Point towards base pole
            latRadiusVec = dx::XMVectorScale(latRadiusVec, radius);
            float ringRadius = cosf(latAngle) * radius;

            // Calculate center of the ring for this latitude
            dx::XMVECTOR latCenter = dx::XMVectorAdd(baseVec, dx::XMVectorScale(axisNormVec, -sinf(latAngle) * radius)); // Move down from base

            for (int iLong = 0; iLong < longDiv; ++iLong)
            {
                float longAngle = longitudeAngle * iLong;
                dx::XMVECTOR ringOffset = dx::XMVectorAdd(dx::XMVectorScale(perpX, cosf(longAngle)), dx::XMVectorScale(perpY, sinf(longAngle)));
                ringOffset = dx::XMVectorScale(ringOffset, ringRadius);

                dx::XMFLOAT3 capPos;
                dx::XMStoreFloat3(&capPos, dx::XMVectorAdd(latCenter, ringOffset));
                vb.EmplaceBack(capPos);
            }
        }
        // Add bottom pole vertex
        const auto iSouthPole = (unsigned short)vb.Size();
        {
            dx::XMFLOAT3 southPos;
            dx::XMStoreFloat3(&southPos, dx::XMVectorSubtract(baseVec, dx::XMVectorScale(axisNormVec, radius)));
            vb.EmplaceBack(southPos);
        }


        // Top Hemisphere (excluding the top ring itself)
        unsigned short topCapStartIdx = (unsigned short)vb.Size();
        for (int iLat = 1; iLat < latDiv; ++iLat) // Start from 1 
        {
            float latAngle = latitudeAngle * iLat;
            float ringRadius = cosf(latAngle) * radius;
            dx::XMVECTOR latCenter = dx::XMVectorAdd(tipVec, dx::XMVectorScale(axisNormVec, sinf(latAngle) * radius)); // Move up from tip

            for (int iLong = 0; iLong < longDiv; ++iLong)
            {
                float longAngle = longitudeAngle * iLong;
                dx::XMVECTOR ringOffset = dx::XMVectorAdd(dx::XMVectorScale(perpX, cosf(longAngle)), dx::XMVectorScale(perpY, sinf(longAngle)));
                ringOffset = dx::XMVectorScale(ringOffset, ringRadius);

                dx::XMFLOAT3 capPos;
                dx::XMStoreFloat3(&capPos, dx::XMVectorAdd(latCenter, ringOffset));
                vb.EmplaceBack(capPos);
            }
        }
        // Add top pole vertex
        const auto iNorthPole = (unsigned short)vb.Size();
        {
            dx::XMFLOAT3 northPos;
            dx::XMStoreFloat3(&northPos, dx::XMVectorAdd(tipVec, dx::XMVectorScale(axisNormVec, radius)));
            vb.EmplaceBack(northPos);
        }


        // --- Generate Indices ---

        // -- Cylinder Body Indices --
        for (unsigned short iLong = 0; iLong < longDiv; ++iLong)
        {
            unsigned short nextILong = (iLong + 1) % longDiv;
            unsigned short currentBase = baseRingStartIdx + iLong;
            unsigned short currentTop = topRingStartIdx + iLong;
            unsigned short nextBase = baseRingStartIdx + nextILong;
            unsigned short nextTop = topRingStartIdx + nextILong;

            indices.push_back(currentBase);
            indices.push_back(nextTop);
            indices.push_back(currentTop);

            indices.push_back(currentBase);
            indices.push_back(nextBase);
            indices.push_back(nextTop);
        }

        // -- Bottom Hemisphere Indices --
        // Connect base ring to the first cap ring
        for (unsigned short iLong = 0; iLong < longDiv; ++iLong) {
            unsigned short nextILong = (iLong + 1) % longDiv;
            unsigned short currentBase = baseRingStartIdx + iLong;
            unsigned short nextBase = baseRingStartIdx + nextILong;
            unsigned short currentCapRing = bottomCapStartIdx + iLong;
            unsigned short nextCapRing = bottomCapStartIdx + nextILong;

            indices.push_back(currentBase);
            indices.push_back(currentCapRing);
            indices.push_back(nextBase);

            indices.push_back(nextBase);
            indices.push_back(currentCapRing);
            indices.push_back(nextCapRing);
        }
        // Connect intermediate cap rings
        for (unsigned short iLat = 0; iLat < latDiv - 2; iLat++) {
            for (unsigned short iLong = 0; iLong < longDiv; iLong++) {
                unsigned short nextILong = (iLong + 1) % longDiv;
                unsigned short current = bottomCapStartIdx + iLat * longDiv + iLong;
                unsigned short nextLon = bottomCapStartIdx + iLat * longDiv + nextILong;
                unsigned short nextLat = bottomCapStartIdx + (iLat + 1) * longDiv + iLong;
                unsigned short nextLatLon = bottomCapStartIdx + (iLat + 1) * longDiv + nextILong;

                indices.push_back(current);
                indices.push_back(nextLat);
                indices.push_back(nextLon);

                indices.push_back(nextLon);
                indices.push_back(nextLat);
                indices.push_back(nextLatLon);
            }
        }
        // Connect last cap ring to south pole
        for (unsigned short iLong = 0; iLong < longDiv; ++iLong) {
            unsigned short nextILong = (iLong + 1) % longDiv;
            unsigned short current = bottomCapStartIdx + (latDiv - 2) * longDiv + iLong;
            unsigned short next = bottomCapStartIdx + (latDiv - 2) * longDiv + nextILong;
            indices.push_back(current);
            indices.push_back(iSouthPole);
            indices.push_back(next);
        }

        // -- Top Hemisphere Indices -- (Similar logic, reversed winding for normals if needed)
         // Connect top ring to the first cap ring
        for (unsigned short iLong = 0; iLong < longDiv; ++iLong) {
            unsigned short nextILong = (iLong + 1) % longDiv;
            unsigned short currentTop = topRingStartIdx + iLong;
            unsigned short nextTop = topRingStartIdx + nextILong;
            unsigned short currentCapRing = topCapStartIdx + iLong;
            unsigned short nextCapRing = topCapStartIdx + nextILong;

            indices.push_back(currentTop);
            indices.push_back(nextTop); // Reversed winding for top cap
            indices.push_back(currentCapRing);

            indices.push_back(currentCapRing);
            indices.push_back(nextTop); // Reversed winding
            indices.push_back(nextCapRing);
        }
        // Connect intermediate cap rings
        for (unsigned short iLat = 0; iLat < latDiv - 2; iLat++) {
            for (unsigned short iLong = 0; iLong < longDiv; iLong++) {
                unsigned short nextILong = (iLong + 1) % longDiv;
                unsigned short current = topCapStartIdx + iLat * longDiv + iLong;
                unsigned short nextLon = topCapStartIdx + iLat * longDiv + nextILong;
                unsigned short nextLat = topCapStartIdx + (iLat + 1) * longDiv + iLong;
                unsigned short nextLatLon = topCapStartIdx + (iLat + 1) * longDiv + nextILong;

                indices.push_back(current);
                indices.push_back(nextLon); // Reversed winding
                indices.push_back(nextLat);

                indices.push_back(nextLat);
                indices.push_back(nextLon); // Reversed winding
                indices.push_back(nextLatLon);
            }
        }
        // Connect last cap ring to north pole
        for (unsigned short iLong = 0; iLong < longDiv; ++iLong) {
            unsigned short nextILong = (iLong + 1) % longDiv;
            unsigned short current = topCapStartIdx + (latDiv - 2) * longDiv + iLong;
            unsigned short next = topCapStartIdx + (latDiv - 2) * longDiv + nextILong;
            indices.push_back(current);
            indices.push_back(next); // Reversed winding
            indices.push_back(iNorthPole);
        }

        return { std::move(vb), std::move(indices) };
    }

    // Convenience overload
    static IndexedTriangleList Make(DirectX::XMFLOAT3 base, DirectX::XMFLOAT3 tip, float radius, std::optional<Dvtx::VertexLayout> layout = std::nullopt, int latDiv = 6, int longDiv = 12)
    {
        using Element = Dvtx::VertexLayout::ElementType;
        if (!layout)
        {
            layout = Dvtx::VertexLayout{}.Append(Element::Position3D);
        }
        tip.y = (tip.y - radius);
		base.y = (base.y + radius);
        // Make sure latDiv >= 3 for hemisphere generation
        return MakeTesselated(std::move(*layout), base, tip, radius, std::max(latDiv, 3), longDiv);
    }
};
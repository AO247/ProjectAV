#pragma once
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>
#include <Jolt/Physics/Collision/PhysicsMaterial.h>
#include <Jolt/Physics/Collision/PhysicsMaterialSimple.h>
#include <Jolt/Physics/Collision/Shape/Shape.h>
#include <Jolt/Physics/Collision/Raycast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/NarrowPhaseQuery.h>
#include <Jolt/Physics/Collision/CollisionGroup.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Constraints/FixedConstraint.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include "vector"
#include "SimpleMath.h"
#include "ModelComponent.h"
#include "Node.h"
#include "Trigger.h"
#include "MyContactListener.h"
#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseQuery.h>
#include <Jolt/Math/Vec3.h>
#include <Jolt/Geometry/OrientedBox.h>


using namespace DirectX::SimpleMath;
using namespace JPH;

class Trigger;
class Rigidbody;
class MyContactListener;

class PhysicsCommon
{
public:
	static JPH::PhysicsSystem* physicsSystem;

    static JPH::TriangleList MakeTriangleList(std::vector<ModelComponent::Triangle> triangles)
    {
        JPH::TriangleList result;
        for (int i = 0; i < triangles.size(); i++)
        {
            JPH::Triangle t(Float3(triangles[i].v0.x, triangles[i].v0.y, triangles[i].v0.z),
                            Float3(triangles[i].v1.x, triangles[i].v1.y, triangles[i].v1.z),
                            Float3(triangles[i].v2.x, triangles[i].v2.y, triangles[i].v2.z));
            result.push_back(t);
        }
        return result;
    }
    static PhysicsMaterialList MakeMaterialList(PhysicsMaterialRefC material, int size)
    {
        PhysicsMaterialList result;

        for (int i = 0; i < size; i++)
        {
            result.push_back(material);
        }

        return result;
    }
    static AABox ComputeAABBFromOrientedBox(const Vec3& halfExtent, const Mat44& transform)
    {
        // 8 naro¿ników w lokalnych wspó³rzêdnych
        std::vector<Vec3> corners = {
            Vec3(-halfExtent.GetX(), -halfExtent.GetY(), -halfExtent.GetZ()),
            Vec3(halfExtent.GetX(), -halfExtent.GetY(), -halfExtent.GetZ()),
            Vec3(-halfExtent.GetX(),  halfExtent.GetY(), -halfExtent.GetZ()),
            Vec3(halfExtent.GetX(),  halfExtent.GetY(), -halfExtent.GetZ()),
            Vec3(-halfExtent.GetX(), -halfExtent.GetY(),  halfExtent.GetZ()),
            Vec3(halfExtent.GetX(), -halfExtent.GetY(),  halfExtent.GetZ()),
            Vec3(-halfExtent.GetX(),  halfExtent.GetY(),  halfExtent.GetZ()),
            Vec3(halfExtent.GetX(),  halfExtent.GetY(),  halfExtent.GetZ())
        };

        // Transformuj naro¿niki do œwiata
        Vec3 min = transform * corners[0];
        Vec3 max = min;

        for (size_t i = 1; i < corners.size(); ++i)
        {
            Vec3 worldCorner = transform * corners[i];
			min = Vec3::sMin(min, worldCorner);
			max = Vec3::sMax(max, worldCorner);
        }

        return AABox(min, max);
    }
    static BodyIDVector OverlapBox(Vec3 center, Vec3 halfExtent, Quat rotation)
    {
        AABox aabb(center - halfExtent, center + halfExtent);
		//AABox aabb = ComputeAABBFromOrientedBox(halfExtent, Mat44::sRotationTranslation(rotation, center));
        AllHitCollisionCollector<CollideShapeBodyCollector> collector;
        // Lambda-callback do zbierania wyników
        physicsSystem->GetBroadPhaseQuery().CollideAABox(aabb, collector);
        return collector.mHits;
    }


};
JPH_SUPPRESS_WARNINGS

static void TraceImpl(const char* inFMT, ...)
{
    // Format the message
    va_list list;
    va_start(list, inFMT);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), inFMT, list);
    va_end(list);

    // Print to the TTY
    //cout << buffer << endl;
}

#ifdef JPH_ENABLE_ASSERTS

// Callback for asserts, connect this to your own assert handler if you have one
static bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, uint inLine)
{
    // Print to the TTY
    //cout << inFile << ":" << inLine << ": (" << inExpression << ") " << (inMessage != nullptr ? inMessage : "") << endl;

    // Breakpoint
    return true;
};


#endif // JPH_ENABLE_ASSERTS

namespace Layers
{
    static constexpr ObjectLayer NON_MOVING = 0;
    static constexpr ObjectLayer MOVING = 1;
    static constexpr ObjectLayer GROUND = 2;
    static constexpr ObjectLayer TRIGGER = 3;
	static constexpr ObjectLayer WALL = 4;
    static constexpr ObjectLayer PLAYER = 5;
    static constexpr ObjectLayer NUM_LAYERS = 6;
};

class ObjectLayerPairFilterImpl : public ObjectLayerPairFilter
{
public:
    virtual bool ShouldCollide(ObjectLayer inObject1, ObjectLayer inObject2) const override
    {
        switch (inObject1)
        {
        case Layers::NON_MOVING:
            return inObject2 == Layers::MOVING; // Non moving only collides with moving
        case Layers::MOVING:
            return true;
        case Layers::TRIGGER:
            return true;
        case Layers::GROUND:
            return true;
        case Layers::WALL:
            return true;
		case Layers::PLAYER:
            return true;
        default:
            return false;
        }
    }
};

namespace BroadPhaseLayers
{
    static constexpr BroadPhaseLayer NON_MOVING(0);
    static constexpr BroadPhaseLayer MOVING(1);
	static constexpr BroadPhaseLayer GROUND(2);
    static constexpr BroadPhaseLayer TRIGGER(3);
	static constexpr BroadPhaseLayer WALL(4);
    static constexpr BroadPhaseLayer PLAYER(5);

    static constexpr uint NUM_LAYERS(6);
};

class BPLayerInterfaceImpl final : public BroadPhaseLayerInterface
{
public:
    BPLayerInterfaceImpl()
    {
        // Create a mapping table from object to broad phase layer
        mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
        mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
		mObjectToBroadPhase[Layers::GROUND] = BroadPhaseLayers::GROUND;
        mObjectToBroadPhase[Layers::TRIGGER] = BroadPhaseLayers::TRIGGER;
		mObjectToBroadPhase[Layers::WALL] = BroadPhaseLayers::WALL;
        mObjectToBroadPhase[Layers::PLAYER] = BroadPhaseLayers::PLAYER;
    }

    virtual uint GetNumBroadPhaseLayers() const override
    {
        return BroadPhaseLayers::NUM_LAYERS;
    }

    virtual BroadPhaseLayer GetBroadPhaseLayer(ObjectLayer inLayer) const override
    {
        JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
        return mObjectToBroadPhase[inLayer];
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    virtual const char* GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override
    {
        switch ((BroadPhaseLayer::Type)inLayer)
        {
        case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
        case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		return "MOVING";
		case (BroadPhaseLayer::Type)BroadPhaseLayers::GROUND:		return "GROUND";
        case (BroadPhaseLayer::Type)BroadPhaseLayers::TRIGGER:		return "TRIGGER";
		case (BroadPhaseLayer::Type)BroadPhaseLayers::WALL:		    return "WALL";
		case (BroadPhaseLayer::Type)BroadPhaseLayers::PLAYER:		return "PLAYER";
        default:													JPH_ASSERT(false); return "INVALID";
        }
    }
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
    BroadPhaseLayer	mObjectToBroadPhase[Layers::NUM_LAYERS];
};

class ObjectVsBroadPhaseLayerFilterImpl : public ObjectVsBroadPhaseLayerFilter
{
public:
    virtual bool ShouldCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) const override
    {
        switch (inLayer1)
        {
        case Layers::NON_MOVING:
            return inLayer2 == BroadPhaseLayers::MOVING;
        case Layers::MOVING:
            return true;
        case Layers::TRIGGER:
            return true;
        case Layers::GROUND:
            return true;
        case Layers::WALL:
            return true;
		case Layers::PLAYER:
			return true;
        default:
            return false;
        }
    }
};

class MultipleBroadPhaseLayerFilter : public BroadPhaseLayerFilter
{
public:

    MultipleBroadPhaseLayerFilter(std::vector<BroadPhaseLayer> inLayer) :
        mLayer(inLayer)
    {
    }

    virtual bool ShouldCollide(BroadPhaseLayer inLayer) const override
    {
        bool layerFound = false;
        for (BroadPhaseLayer layer : mLayer)
        {
            if (inLayer == layer)
            {
                layerFound = true;
                break;
            }
        }
        return layerFound;
    }

private:
    std::vector<BroadPhaseLayer> mLayer;
};

class IgnoreSpecifiedBroadPhaseLayerFilter : public BroadPhaseLayerFilter
{
public:

    IgnoreSpecifiedBroadPhaseLayerFilter(BroadPhaseLayer inLayer) :
        mLayer(inLayer)
    {
    }

    virtual bool ShouldCollide(BroadPhaseLayer inLayer) const override
    {
        return mLayer != inLayer;
    }

private:
    BroadPhaseLayer	mLayer;
};

class IgnoreMultipleBroadPhaseLayerFilter : public BroadPhaseLayerFilter
{
public:

    IgnoreMultipleBroadPhaseLayerFilter(std::vector<BroadPhaseLayer> inLayer) :
        mLayer(inLayer)
    {
    }

    virtual bool ShouldCollide(BroadPhaseLayer inLayer) const override
    {
        bool layerFound = false;
        for (BroadPhaseLayer layer : mLayer)
        {
            if (inLayer == layer)
            {
                layerFound = true;
                break;
            }
        }
        return !layerFound;
    }

private:
    std::vector<BroadPhaseLayer> mLayer;
};

class MultipleObjectLayerFilter : public ObjectLayerFilter
{
public:

    MultipleObjectLayerFilter(ObjectLayer inLayer) :
        mLayer(inLayer)
    {
    }

    virtual bool ShouldCollide(ObjectLayer inLayer) const override
    {
        bool layerFound = false;
        for (ObjectLayer layer : mLayer)
        {
            if (inLayer == layer)
            {
                layerFound = true;
                break;
            }
        }
        return layerFound;
    }

private:
    std::vector<ObjectLayer> mLayer;
};

class IgnoreSpecifiedObjectLayerFilter : public ObjectLayerFilter
{
public:

    IgnoreSpecifiedObjectLayerFilter(ObjectLayer inLayer) :
        mLayer(inLayer)
    {
    }

    virtual bool ShouldCollide(ObjectLayer inLayer) const override
    {
        return mLayer != inLayer;
    }

private:
    ObjectLayer	mLayer;
};

class IgnoreMultipleObjectLayerFilter : public ObjectLayerFilter
{
public:

    IgnoreMultipleObjectLayerFilter(std::vector<ObjectLayer> inLayer) :
        mLayer(inLayer)
    {
    }

    virtual bool ShouldCollide(ObjectLayer inLayer) const override
    {
        bool layerFound = false;
        for (ObjectLayer layer : mLayer)
        {
            if (inLayer == layer)
            {
                layerFound = true;
                break;
            }
        }
        return !layerFound;
    }

private:
    std::vector<ObjectLayer> mLayer;
};
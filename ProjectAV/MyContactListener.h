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
#include "PhysicsCommon.h"
#include <map>

using namespace JPH;

class PhysicsCommon;
class Trigger;

class MyContactListener : public ContactListener
{
public:
    // See: ContactListener
    virtual ValidateResult OnContactValidate(const Body& inBody1, const Body& inBody2, RVec3Arg inBaseOffset, const CollideShapeResult& inCollisionResult) override
    {
        //cout << "Contact validate callback" << endl;

        // Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
        return ValidateResult::AcceptAllContactsForThisBodyPair;
    }

    virtual void OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override;

    virtual void OnContactPersisted(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings) override
    {
        //cout << "A contact was persisted" << endl;
    }

    virtual void OnContactRemoved(const SubShapeIDPair& inSubShapePair) override;

    void AddTrigger(BodyID id);
    void AddRigidbody(BodyID id);

    void ExecuteTriggerActivationQueue();
    void ExecuteCollisionActivationQueue();
private:
    enum ActivationType
    {
        ENTER,
        EXIT
    };

    struct TriggerActivationEvent
    {
        TriggerActivationEvent(BodyID trigger, BodyID activator, ActivationType activationType) : 
            trigger(trigger), 
            activator(activator), 
            activationType(activationType) 
        {}
        BodyID trigger;
        BodyID activator;
        ActivationType activationType;
    };

    std::map<BodyID, std::map<BodyID, int>> contacts;
    std::map<BodyID, std::map<BodyID, int>> collisionContacts;
    std::vector<TriggerActivationEvent> triggerActivationQueue;
    std::vector<TriggerActivationEvent> collisionActivationQueue;
};
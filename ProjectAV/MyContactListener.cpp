#include "MyContactListener.h"

void MyContactListener::OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings)
{
    BodyID body1ID = inBody1.GetID();
    BodyID body2ID = inBody2.GetID();

    // TRIGGERS
    if (contacts.count(body1ID) != 0)
    {
        contacts[body1ID][body2ID]++;
        if (contacts[body1ID][body2ID] == 1)
        {
            triggerActivationQueue.push_back(TriggerActivationEvent(body1ID, body2ID, ENTER));
        }
    }
    if (contacts.count(body2ID) != 0)
    {
        contacts[body2ID][body1ID]++;
        if (contacts[body2ID][body1ID] == 1)
        {
            triggerActivationQueue.push_back(TriggerActivationEvent(body2ID, body1ID, ENTER));
        }
    }

    // RIGIDBODIES
    if (collisionContacts.count(body1ID) != 0)
    {
        collisionContacts[body1ID][body2ID]++;
        if (collisionContacts[body1ID][body2ID] == 1)
        {
            collisionActivationQueue.push_back(TriggerActivationEvent(body1ID, body2ID, ENTER));
        }
    }
    if (collisionContacts.count(body2ID) != 0)
    {
        collisionContacts[body2ID][body1ID]++;
        if (collisionContacts[body2ID][body1ID] == 1)
        {
            collisionActivationQueue.push_back(TriggerActivationEvent(body2ID, body1ID, ENTER));
        }
    }

    /*if (inBody1.IsSensor() || inBody2.IsSensor())
    {
        if (inBody1.IsSensor())
        {
            Node* other = reinterpret_cast<Node*>(inBody2.GetUserData());
            reinterpret_cast<Node*>(inBody1.GetUserData())->GetComponent<Trigger>()->AddContactPoint(other);
        }
        if (inBody2.IsSensor())
        {
            Node* other = reinterpret_cast<Node*>(inBody1.GetUserData());
            reinterpret_cast<Node*>(inBody2.GetUserData())->GetComponent<Trigger>()->AddContactPoint(other);
        }
    }*/
}

void MyContactListener::OnContactRemoved(const SubShapeIDPair& inSubShapePair)
{
    BodyID body1ID = inSubShapePair.GetBody1ID();
    BodyID body2ID = inSubShapePair.GetBody2ID();

    // TRIGGERS
    if (contacts.count(body1ID) != 0)
    {
        contacts[body1ID][body2ID]--;
        if (contacts[body1ID][body2ID] == 0)
        {
            triggerActivationQueue.push_back(TriggerActivationEvent(body1ID, body2ID, EXIT));
            contacts[body1ID].erase(body2ID);
        }
    }
    if (contacts.count(body2ID) != 0)
    {
        contacts[body2ID][body1ID]--;
        if (contacts[body2ID][body1ID] == 0)
        {
            triggerActivationQueue.push_back(TriggerActivationEvent(body2ID, body1ID, EXIT));
            contacts[body2ID].erase(body1ID);
        }
    }

    // RIGIDBODIES
    if (collisionContacts.count(body1ID) != 0)
    {
        collisionContacts[body1ID][body2ID]--;
        if (collisionContacts[body1ID][body2ID] == 0)
        {
            collisionActivationQueue.push_back(TriggerActivationEvent(body1ID, body2ID, EXIT));
            //collisionContacts[body1ID].erase(body2ID);
        }
    }
    if (collisionContacts.count(body2ID) != 0)
    {
        collisionContacts[body2ID][body1ID]--;
        if (collisionContacts[body2ID][body1ID] == 0)
        {
            collisionActivationQueue.push_back(TriggerActivationEvent(body2ID, body1ID, EXIT));
            //collisionContacts[body2ID].erase(body1ID);
        }
    }

    /*BodyID body1ID = inSubShapePair.GetBody1ID();
    BodyID body2ID = inSubShapePair.GetBody2ID();

    if (reinterpret_cast<Node*>(PhysicsCommon::physicsSystem->GetBodyInterface().GetUserData(body1ID))->GetComponent<Trigger>() != nullptr ||
        reinterpret_cast<Node*>(PhysicsCommon::physicsSystem->GetBodyInterface().GetUserData(body2ID))->GetComponent<Trigger>() != nullptr)
    {
        if (reinterpret_cast<Node*>(PhysicsCommon::physicsSystem->GetBodyInterface().GetUserData(body1ID))->GetComponent<Trigger>() != nullptr)
        {
            Node* other = reinterpret_cast<Node*>(PhysicsCommon::physicsSystem->GetBodyInterface().GetUserData(body2ID));
            reinterpret_cast<Node*>(PhysicsCommon::physicsSystem->GetBodyInterface().GetUserData(body1ID))->GetComponent<Trigger>()->RemoveContactPoint(other);
        }
        if (reinterpret_cast<Node*>(PhysicsCommon::physicsSystem->GetBodyInterface().GetUserData(body2ID))->GetComponent<Trigger>() != nullptr)
        {
            Node* other = reinterpret_cast<Node*>(PhysicsCommon::physicsSystem->GetBodyInterface().GetUserData(body1ID));
            reinterpret_cast<Node*>(PhysicsCommon::physicsSystem->GetBodyInterface().GetUserData(body2ID))->GetComponent<Trigger>()->RemoveContactPoint(other);
        }
    }*/
}

void MyContactListener::AddTrigger(BodyID id)
{
    contacts[id] = {};
}

void MyContactListener::AddRigidbody(BodyID id)
{
    collisionContacts[id] = {};
}

void MyContactListener::ExecuteTriggerActivationQueue()
{
    for (int i = 0; i < triggerActivationQueue.size(); i++)
    {
        Node* triggerNode = reinterpret_cast<Node*>(PhysicsCommon::physicsSystem->GetBodyInterface().GetUserData(triggerActivationQueue[i].trigger));
        Node* activatorNode = reinterpret_cast<Node*>(PhysicsCommon::physicsSystem->GetBodyInterface().GetUserData(triggerActivationQueue[i].activator));
        if (triggerActivationQueue[i].activationType == ENTER)
        {
            const auto& components = triggerNode->GetComponents();
            for (const auto& component : components) {
                component->OnTriggerEnter(activatorNode);
            }
        }
        if (triggerActivationQueue[i].activationType == EXIT)
        {
            const auto& components = triggerNode->GetComponents();
            for (const auto& component : components) {
                component->OnTriggerExit(activatorNode);
            }
        }
    }
    triggerActivationQueue.clear();
}

void MyContactListener::ExecuteCollisionActivationQueue()
{
    for (int i = 0; i < collisionActivationQueue.size(); i++)
    {
        Node* triggerNode = reinterpret_cast<Node*>(PhysicsCommon::physicsSystem->GetBodyInterface().GetUserData(collisionActivationQueue[i].trigger));
        Node* activatorNode = reinterpret_cast<Node*>(PhysicsCommon::physicsSystem->GetBodyInterface().GetUserData(collisionActivationQueue[i].activator));
        if (collisionActivationQueue[i].activationType == ENTER)
        {
            const auto& components = triggerNode->GetComponents();
            for (const auto& component : components) {
                component->OnCollisionEnter(activatorNode);
            }
        }
        if (collisionActivationQueue[i].activationType == EXIT)
        {
            const auto& components = triggerNode->GetComponents();
            for (const auto& component : components) {
                component->OnCollisionExit(activatorNode);
            }
        }
    }
    collisionActivationQueue.clear();
}

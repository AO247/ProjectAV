#include "MyContactListener.h"
#include <map>
#include <vector>
void MyContactListener::OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings)
{
    BodyID body1ID = inBody1.GetID();
    BodyID body2ID = inBody2.GetID();

    lock_guard lock(mMutex);

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
}

void MyContactListener::OnContactPersisted(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings)
{
    BodyID body1ID = inBody1.GetID();
    BodyID body2ID = inBody2.GetID();

    lock_guard lock(mMutex);

    bool body1AlreadyInQueue = false;
    bool body2AlreadyInQueue = false;
    for (int i = 0; i < triggerActivationQueue.size(); i++)
    {
        if (contacts.count(body1ID) != 0 &&
            triggerActivationQueue[i].trigger == body1ID &&
            triggerActivationQueue[i].activator == body2ID &&
            triggerActivationQueue[i].activationType == STAY)
        {
            body1AlreadyInQueue = true;
        }
        if (contacts.count(body2ID) &&
            triggerActivationQueue[i].trigger == body2ID && 
            triggerActivationQueue[i].activator == body1ID &&
            triggerActivationQueue[i].activationType == STAY)
        {
            body2AlreadyInQueue = true;
        }
    }
    if (contacts.count(body1ID) != 0 && !body1AlreadyInQueue)
    {
        triggerActivationQueue.push_back(TriggerActivationEvent(body1ID, body2ID, STAY));
    }
    if (contacts.count(body2ID) != 0 && !body2AlreadyInQueue)
    {
        triggerActivationQueue.push_back(TriggerActivationEvent(body2ID, body1ID, STAY));
    }

    body1AlreadyInQueue = false;
    body2AlreadyInQueue = false;
    for (int i = 0; i < collisionActivationQueue.size(); i++)
    {
        if (collisionContacts.count(body1ID) != 0 &&
            collisionActivationQueue[i].trigger == body1ID &&
            collisionActivationQueue[i].activator == body2ID &&
            collisionActivationQueue[i].activationType == STAY)
        {
            body1AlreadyInQueue = true;
        }
        if (collisionContacts.count(body2ID) &&
            collisionActivationQueue[i].trigger == body2ID &&
            collisionActivationQueue[i].activator == body1ID &&
            collisionActivationQueue[i].activationType == STAY)
        {
            body2AlreadyInQueue = true;
        }
    }
    if (collisionContacts.count(body1ID) != 0 && !body1AlreadyInQueue)
    {
        collisionActivationQueue.push_back(TriggerActivationEvent(body1ID, body2ID, STAY));
    }
    if (collisionContacts.count(body2ID) != 0 && !body2AlreadyInQueue)
    {
        collisionActivationQueue.push_back(TriggerActivationEvent(body2ID, body1ID, STAY));
    }
}

void MyContactListener::OnContactRemoved(const SubShapeIDPair& inSubShapePair)
{
    BodyID body1ID = inSubShapePair.GetBody1ID();
    BodyID body2ID = inSubShapePair.GetBody2ID();

    lock_guard lock(mMutex);

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
            collisionContacts[body1ID].erase(body2ID);
        }
    }
    if (collisionContacts.count(body2ID) != 0)
    {
        collisionContacts[body2ID][body1ID]--;
        if (collisionContacts[body2ID][body1ID] == 0)
        {
            collisionActivationQueue.push_back(TriggerActivationEvent(body2ID, body1ID, EXIT));
            collisionContacts[body2ID].erase(body1ID);
        }
    }
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
    // Mapa do agregacji zdarzeñ STAY.
    // Klucz: BodyID triggera.
    // Wartoœæ: Lista wskaŸników na Node'y aktywatorów.
    std::map<BodyID, std::vector<Node*>> stayEventsMap;

    for (const auto& event : triggerActivationQueue)
    {
        Node* triggerNode = reinterpret_cast<Node*>(PhysicsCommon::physicsSystem->GetBodyInterface().GetUserData(event.trigger));
        Node* activatorNode = reinterpret_cast<Node*>(PhysicsCommon::physicsSystem->GetBodyInterface().GetUserData(event.activator));

        if (triggerNode == nullptr || activatorNode == nullptr) continue;
        if (!PhysicsCommon::physicsSystem->GetBodyInterface().IsAdded(event.trigger)) continue;

        const auto& components = triggerNode->GetComponents();

        if (event.activationType == ENTER)
        {
            for (const auto& component : components) {
                component->OnTriggerEnter(activatorNode);
            }
        }
        else if (event.activationType == STAY)
        {
            // Zamiast od razu wywo³ywaæ, dodaj do naszej mapy
            stayEventsMap[event.trigger].push_back(activatorNode);
        }
        else if (event.activationType == EXIT)
        {
            for (const auto& component : components) {
                component->OnTriggerExit(activatorNode);
            }
        }
    }

    // Teraz, po przetworzeniu ca³ej kolejki, przejdŸ przez nasz¹ mapê
    // i wywo³aj nowe OnTriggerStay dla ka¿dego triggera.
    for (const auto& pair : stayEventsMap)
    {
        const BodyID& triggerBodyID = pair.first;
        const std::vector<Node*>& activators = pair.second;

        // Pobierz triggerNode jeszcze raz na podstawie BodyID
        Node* triggerNode = reinterpret_cast<Node*>(PhysicsCommon::physicsSystem->GetBodyInterface().GetUserData(triggerBodyID));
        if (triggerNode == nullptr) continue;

        const auto& components = triggerNode->GetComponents();
        for (const auto& component : components)
        {
            // Wywo³aj now¹ wersjê metody z ca³¹ list¹!
            component->OnTriggerStay(activators);
        }
    }

    // Na koñcu wyczyœæ kolejkê
    triggerActivationQueue.clear(); // U¿yj .clear() zamiast ponownej alokacji
}

void MyContactListener::ExecuteCollisionActivationQueue()
{
  /*  for (int i = 0; i < collisionActivationQueue.size(); i++)
    {
        for (int j = 0; j < collisionDeletionQueue.size(); j++)
        {
            if (collisionDeletionQueue[j] == collisionActivationQueue[i].trigger)
            {
                collisionActivationQueue.erase(collisionActivationQueue.begin() + i);
            }
        }
    }*/
    for (int i = 0; i < collisionActivationQueue.size(); i++)
    {
        Node* triggerNode = reinterpret_cast<Node*>(PhysicsCommon::physicsSystem->GetBodyInterface().GetUserData(collisionActivationQueue[i].trigger));
        Node* activatorNode = reinterpret_cast<Node*>(PhysicsCommon::physicsSystem->GetBodyInterface().GetUserData(collisionActivationQueue[i].activator));
        if (triggerNode == nullptr || activatorNode == nullptr) continue;
        if (collisionActivationQueue[i].activationType == ENTER)
        {
            const auto& components = triggerNode->GetComponents();
            for (const auto& component : components) {
                component->OnCollisionEnter(activatorNode);
            }
        }
        if (collisionActivationQueue[i].activationType == STAY)
        {
            const auto& components = triggerNode->GetComponents();
            for (const auto& component : components) {
                component->OnCollisionStay(activatorNode);
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
    collisionActivationQueue = std::vector<TriggerActivationEvent>();
    //collisionDeletionQueue = std::vector<BodyID>();
}

void MyContactListener::RemoveTriggerData(BodyID id)
{
    /*for (int i = 0; i < triggerActivationQueue.size(); i++)
    {
        if (triggerActivationQueue[i].trigger == id)
        {
            triggerActivationQueue.erase(triggerActivationQueue.begin() + i);
        }
    }*/
    contacts.erase(id);
    std::map<BodyID, std::map<BodyID, int>>::iterator it;
    for (it = contacts.begin(); it != contacts.end(); it++)
    {
        it->second.erase(id);
        //triggerActivationQueue.push_back(TriggerActivationEvent(it->first, id, EXIT));
    }

    //triggerDeletionQueue.push_back(id);
}

void MyContactListener::RemoveRigidbodyData(BodyID id)
{
    /*for (int i = 0; i < collisionActivationQueue.size(); i++)
    {
        if (collisionActivationQueue[i].trigger == id)
        {
            collisionActivationQueue.erase(collisionActivationQueue.begin() + i);
        }
    }*/
    collisionContacts.erase(id);
    std::map<BodyID, std::map<BodyID, int>>::iterator it;
    for (it = collisionContacts.begin(); it != collisionContacts.end(); it++)
    {
        it->second.erase(id);
        //collisionActivationQueue.push_back(TriggerActivationEvent(it->first, id, EXIT));
    }

    //collisionDeletionQueue.push_back(id);
}

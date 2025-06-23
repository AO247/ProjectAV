#include "FallDamage.h"
#include "Node.h"       
#include "PrefabManager.h"

namespace dx = DirectX;
FallDamage::FallDamage(Node* owner, Node* player)
	: Component(owner), player(player)
{
	rigidbody = player->GetComponent<Rigidbody>();
    playerController = player->GetComponent<PlayerController>();

}

void FallDamage::Update(float dt)
{
    if (fallPushCooldown > 0.0f)
    {
        fallPushCooldown -= dt;
    }
    
    if (playerController->grounded && lastVelocity.GetY() < -20.0f)
    {
        if (playerController->enableFallPush && fallPushCooldown <= 0.0f)
        {

            for (int i = 0; i < objects.size(); i++)
            {
                if (objects[i]->tag == "ENEMY" || objects[i]->tag == "BULLET")
                {
                    Vector3 direction = objects[i]->GetWorldPosition() - pOwner->GetWorldPosition();
                    float length = direction.Length();

                    maxDistance = -lastVelocity.GetY() / 4.0f;
                    if (maxDistance < 10.0f) maxDistance = 10.0f;

                    if (length < maxDistance && direction.y < 7.0f && direction.y > -7.0f)
                        direction.Normalize();
                    direction.y = 0.5f;
                    direction.Normalize();
                    float force = maxDistance - length;
                    force *= minFallForce * -lastVelocity.GetY();
                    if (force < minFallForce)
                        force = minFallForce;
                    else if (force > maxFallForce)
                        force = maxFallForce;

                    PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(objects[i]->GetComponent<Rigidbody>()->GetBodyID(),
                        Vec3(direction.x, direction.y, direction.z) * force);
                }
            }
            fallPushCooldown = 1.0f;

            // particle ulepszonego upadku
            // dŸwiêk ulepszonego upadku
        }
        else
        {
            // particle normalnego upadku
            // dŸwiêk normalnego upadku

        }
    }
    lastVelocity = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID());

}

void FallDamage::OnTriggerStay(const std::vector<Node*> others) {
	objects.clear();
	objects = others;
}


void FallDamage::DrawImGuiControls()
{

}
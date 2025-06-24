#include "Bullet.h"
#include <string>
Bullet::Bullet(Node* owner) : Component(owner)
{
	rigidbody = owner->GetComponent<Rigidbody>();
}
void Bullet::Update(float dt)
{
	timer += dt;
	if (timer >= lifeTime) {
		pOwner->Destroy();
	}
}

void Bullet::OnTriggerEnter(Node* object)
{
	if (object == nullptr) return;
	if (object->tag == "TRIGGER"|| object == pOwner || object == ignore) return;
	if (object->tag == "PLAYER") 
	{
		 object->GetComponent<Health>()->TakeDamage(damage);
	}
	if (pushedByPlayer && object->tag == "ENEMY")
	{
		object->GetComponent<Health>()->TakeDamage(0.0f);
	}
	pOwner->Destroy();
}
void Bullet::OnCollisionEnter(Node* object)
{
	if (object == nullptr) return;
	if (object->tag == "TRIGGER" || object == pOwner || object == ignore) return;
	pOwner->Destroy();
}

void Bullet::DrawImGuiControls()
{

}

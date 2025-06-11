#include "BasicAttack.h"
#include "Node.h"       


namespace dx = DirectX;
BasicAttack::BasicAttack(Node* owner, std::string tag)
	: Component(owner, std::move(tag))
{
	attackRange = 5.0f;
}

void BasicAttack::Attack(float dt)
{
	timer += dt;
	if (timer >= wholeAttackTime) {
		attacked = false;
		timer = 0.0f;
		endAttack = true;
		return;
	}
	if (attacked) {
		if (!pOwner->GetComponent<SoundEffectsPlayer>()->isPlaying()) {
			float p = (rand() % 3) + 4;
			pOwner->GetComponent<SoundEffectsPlayer>()->Play(p);
		}
		return;
	}
	if (timer < startDmgTime) {
		return;
	}
	else if (timer > stopDmgTime) {
		return;
	}
	CheckAttack();
}

void BasicAttack::CheckAttack()
{
	if (objects.empty()) return;
	objects[0]->GetComponent<Health>()->TakeDamage(damage);
	Vec3 pos(pOwner->GetWorldPosition().x, objects[0]->GetWorldPosition().y - 0.2f, pOwner->GetWorldPosition().z);
	Vec3 knockDirection = Vec3(objects[0]->GetWorldPosition().x, objects[0]->GetWorldPosition().y, objects[0]->GetWorldPosition().z) - pos;
	knockDirection = knockDirection.Normalized();
	Rigidbody* rb = objects[0]->GetComponent<Rigidbody>();
	PhysicsCommon::physicsSystem->GetBodyInterface().SetLinearVelocity(rb->GetBodyID(), Vec3(0.0f, 0.0f, 0.0f));
	PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(rb->GetBodyID(), knockDirection * knockbackForce);
	attacked = true;
	OutputDebugStringA("\nNormal Attack\n");
}

void BasicAttack::OnTriggerEnter(Node* object) {
	if (object->tag != "PLAYER") return;
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i] == object) return;
	}
	objects.push_back(object);
	OutputDebugStringA(("Enemy OnTriggerEnter: " + object->GetName() + "\n").c_str());
}
void BasicAttack::OnTriggerExit(Node* object) {
	if (object->tag != "PLAYER") return;
	auto it = std::remove(objects.begin(), objects.end(), object);
	if (it != objects.end()) {
		objects.erase(it, objects.end());
	}
	OutputDebugStringA(("Enemy OnTriggerExit: " + object->GetName() + "\n").c_str());
}


void BasicAttack::DrawImGuiControls()
{
	ImGui::Text("Tag: %s", tag.c_str());
	ImGui::InputFloat("Damage", &damage);
	ImGui::InputFloat("Whole Attack Time", &wholeAttackTime);
	ImGui::InputFloat("Start Damage Time", &startDmgTime);
	ImGui::InputFloat("Stop Damage Time", &stopDmgTime);
	ImGui::InputFloat("Knockback Force", &knockbackForce);
	ImGui::InputFloat("Attack Timer", &timer);
	ImGui::Checkbox("Attacked", &attacked);
}
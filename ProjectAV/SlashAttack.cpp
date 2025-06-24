#include "SlashAttack.h"
#include "Node.h"       


namespace dx = DirectX;
SlashAttack::SlashAttack(Node* owner, std::string tag)
	: Component(owner, std::move(tag))
{
}

void SlashAttack::Attack(float dt)
{
	if (timer == 0.0f)
	{
		if (pOwner->GetComponent<SoundEffectsPlayer>()) {
			float randSound = (rand() % 3) + 4;
			pOwner->GetComponent<SoundEffectsPlayer>()->Play(randSound);
		}
		pOwner->GetParent()->GetComponent<AnimationComponent>()->PlayAnimation((rand() % 2 == 0) ? 2 : 4, 0.2f, false);

		//miejsce na animacje !!!
	}
	timer += dt;
	if (timer < stopMovingTime)
	{
		Vec3 direction = Vec3(pOwner->Forward().x, 0.0f, pOwner->Forward().z);
		PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(pOwner->GetParent()->GetComponent<Rigidbody>()->GetBodyID(), direction * moveForce * dt);
	}
	if (timer >= wholeAttackTime) {
		attacked = false;
		timer = 0.0f;
		endAttack = true;
		return;
	}


	if (timer < startDmgTime) {
		return;
	}
	else if (timer > stopDmgTime) {
		return;
	}
	if (attacked) {
		return;
	}
	CheckAttack();
}

void SlashAttack::CheckAttack()
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

void SlashAttack::OnTriggerEnter(Node* object) {
	if (object == nullptr) return;
	if (object->tag != "PLAYER") return;
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i] == object) return;
	}
	objects.push_back(object);
	OutputDebugStringA(("Enemy OnTriggerEnter: " + object->GetName() + "\n").c_str());
}
void SlashAttack::OnTriggerExit(Node* object) {
	if (object == nullptr) return;
	if (object->tag != "PLAYER") return;
	auto it = std::remove(objects.begin(), objects.end(), object);
	if (it != objects.end()) {
		objects.erase(it, objects.end());
	}
	OutputDebugStringA(("Enemy OnTriggerExit: " + object->GetName() + "\n").c_str());
}


void SlashAttack::DrawImGuiControls()
{
	ImGui::Text("Tag: %s", tag.c_str());
	ImGui::InputFloat("Damage", &damage);
	ImGui::InputFloat("Whole Attack Time", &wholeAttackTime);
	ImGui::InputFloat("Start Damage Time", &startDmgTime);
	ImGui::InputFloat("Stop Damage Time", &stopDmgTime);
	ImGui::InputFloat("Knockback Force", &knockbackForce);
	ImGui::InputFloat("Attack Timer", &timer);
	ImGui::InputFloat("Move Force", &moveForce);
	ImGui::Checkbox("Attacked", &attacked);

}
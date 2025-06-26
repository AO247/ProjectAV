#include "RotateAttack.h"
#include "Node.h"       


namespace dx = DirectX;
RotateAttack::RotateAttack(Node* owner, Node* playerNode, std::string tag)
	: Component(owner, std::move(tag)), player(playerNode)
{
}

void RotateAttack::Attack(float dt)
{
	if (timer == 0.0f)
	{
		if (pOwner->GetComponent<SoundEffectsPlayer>()) {
			float randSound = (rand() % 3) + 4;
			pOwner->GetComponent<SoundEffectsPlayer>()->Play(randSound);
		}
		//miejsce na animacje !!!

		pOwner->GetParent()->GetComponent<AnimationComponent>()->PlayAnimation(0, false);


	}
	timer += dt;
	if (timer > startMovingTime && timer < stopMovingTime)
	{
		Node* enemyNode = pOwner->GetParent();
		Rigidbody* enemyRb = enemyNode->GetComponent<Rigidbody>();

		// 1. Oblicz wektor kierunku do gracza i K¥T DOCELOWY (targetYaw)
		Vector3 enemyPos = enemyNode->GetWorldPosition();
		Vector3 playerPos = player->GetWorldPosition();
		Vector3 directionToPlayer = playerPos - enemyPos;
		directionToPlayer.y = 0; // Ignorujemy wysokoœæ
		directionToPlayer.Normalize();

		// atan2f daje nam k¹t w radianach
		float targetYaw = atan2f(directionToPlayer.x, directionToPlayer.z);

		// 2. Pobierz K¥T OBECNY (currentYaw)
		// Zak³adam, ¿e masz funkcjê, która zwraca rotacjê jako k¹ty Eulera
		float currentYaw = enemyNode->GetLocalRotationEuler().y; // Lub GetLocalRotationEuler(), jeœli nie ma rodzica z rotacj¹

		// 3. P³ynnie interpoluj k¹t
		// Oblicz najkrótsz¹ drogê obrotu (np. z -170 do 170 stopni to -20, a nie 340)
		float yawDifference = wrap_angle(targetYaw - currentYaw);

		// Zastosuj tylko czêœæ tej ró¿nicy, aby ruch by³ p³ynny
		// `rotationSpeed` to nowa zmienna, np. 5.0f
		float yawStep = yawDifference * dt * rotationSpeed;

		// Oblicz nowy k¹t
		float newYaw = wrap_angle(currentYaw + yawStep);

		// 4. Ustaw now¹ rotacjê na podstawie nowego k¹ta
		Quat newRotation = Quat::sEulerAngles(Vec3(0.0f, newYaw, 0.0f));
		PhysicsCommon::physicsSystem->GetBodyInterface().SetRotation(
			enemyRb->GetBodyID(),
			newRotation,
			EActivation::Activate
		);

		// 5. Dodaj si³ê/impuls w nowym kierunku
		// Pobierz wektor "do przodu" na podstawie NOWEJ rotacji
		Vec3 forwardDirection = newRotation * Vec3::sAxisZ();

		PhysicsCommon::physicsSystem->GetBodyInterface().AddImpulse(
			enemyRb->GetBodyID(),
			forwardDirection * moveForce
		);
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

void RotateAttack::CheckAttack()
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

void RotateAttack::OnTriggerEnter(Node* object) {
	if (object == nullptr) return;
	if (object->tag != "PLAYER") return;
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i] == object) return;
	}
	objects.push_back(object);
	OutputDebugStringA(("Enemy OnTriggerEnter: " + object->GetName() + "\n").c_str());
}
void RotateAttack::OnTriggerExit(Node* object) {
	if (object == nullptr) return;
	if (object->tag != "PLAYER") return;
	auto it = std::remove(objects.begin(), objects.end(), object);
	if (it != objects.end()) {
		objects.erase(it, objects.end());
	}
	OutputDebugStringA(("Enemy OnTriggerExit: " + object->GetName() + "\n").c_str());
}


void RotateAttack::DrawImGuiControls()
{
	ImGui::Text("Tag: %s", tag.c_str());
	ImGui::InputFloat("Damage", &damage);
	ImGui::InputFloat("Knockback Force", &knockbackForce);
	ImGui::InputFloat("Attack Timer", &timer);
	ImGui::InputFloat("Move Force (Impulse)", &moveForce);
	ImGui::InputFloat("Rotation Speed", &rotationSpeed);
	ImGui::Checkbox("Attacked", &attacked);

}
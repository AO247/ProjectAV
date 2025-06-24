#include "Hands.h"
#include <string>
Hands::Hands(Node* owner) : Component(owner)
{
	rigidbody = pOwner->GetComponent<Rigidbody>();
}

namespace dx = DirectX;

void Hands::Update(float dt)
{
    if (!playerController || !cameraNode) return; // Zabezpieczenie

    // 1. Pobierz wektor ruchu gracza w przestrzeni �wiata
    Vec3 dir = PhysicsCommon::physicsSystem->GetBodyInterface().GetLinearVelocity(rigidbody->GetBodyID());
    //Vector3 moveDirectionWorld = playerController->moveDirection;
    Vector3 moveDirectionWorld = Vector3(dir.GetX(), dir.GetY(), dir.GetZ());
    // Je�li gracz si� nie porusza, r�ce powinny wr�ci� do pozycji bazowej
    if (moveDirectionWorld.LengthSquared() < 0.001f)
    {
        // P�ynny powr�t do centrum zamiast natychmiastowego skoku
        Vector3 currentPos = pOwner->GetLocalPosition();
        pOwner->SetLocalPosition(Vector3::Lerp(currentPos, Vector3::Zero, dt * 10.0f));
        return;
    }

    // 2. Pobierz macierz widoku kamery (transformacja ze �wiata do lokalnej przestrzeni kamery)
    // Macierz widoku to odwrotno�� macierzy �wiata kamery.
    dx::XMMATRIX cameraWorldMatrix = cameraNode->GetWorldTransform();
    dx::XMMATRIX viewMatrix = dx::XMMatrixInverse(nullptr, cameraWorldMatrix);

    // 3. Przetransformuj wektor ruchu ze �wiata do lokalnej przestrzeni kamery
    dx::XMVECTOR moveDirWorldVec = dx::XMLoadFloat3(&moveDirectionWorld);
    // U�ywamy TransformNormal, bo interesuje nas tylko kierunek, a nie przesuni�cie
    dx::XMVECTOR moveDirLocalVec = dx::XMVector3TransformNormal(moveDirWorldVec, viewMatrix);

    Vector3 moveDirectionLocal;
    dx::XMStoreFloat3(&moveDirectionLocal, moveDirLocalVec);

    // 4. U�yj przetransformowanego wektora do obliczenia pozycji r�k
    // Wektor lokalny jest ju� znormalizowany, je�li oryginalny by�
    moveDirectionLocal.Normalize();

    // Chcemy, �eby r�ce przesuwa�y si� w przeciwnym kierunku.
    // O� X w lokalnej przestrzeni kamery to "lewo-prawo".
    // O� Y to "g�ra-d�".
    // Chcemy g��wnie reagowa� na ruch na boki (strafe).
    Vector3 targetPos;
    targetPos.x = -moveDirectionLocal.x * radius * 0.90f;
    targetPos.y = -moveDirectionLocal.y * radius * 1.10f;
    targetPos.z = -moveDirectionLocal.z * radius * 0.70f; // Zazwyczaj nie chcemy przesuwa� r�k w prz�d/ty�

    // 5. P�ynnie interpoluj do nowej pozycji, zamiast skaka�
    Vector3 currentPos = pOwner->GetLocalPosition();
    Vector3 newPos = Vector3::Lerp(currentPos, targetPos, dt * 10.0f); // 10.0f to szybko�� "powrotu"

    pOwner->SetLocalPosition(newPos);
}


void Hands::DrawImGuiControls()
{
	ImGui::InputFloat("Radius", &radius);
}

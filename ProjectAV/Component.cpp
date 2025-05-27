#include "Component.h"
#include <debugapi.h>

void Component::OnTriggerEnter(Node* object)
{
	/*OutputDebugString("do ");
	OutputDebugString(pOwner->GetName().c_str());
	OutputDebugString(" przybyl ");
	OutputDebugString(object->GetName().c_str());
	OutputDebugString("\n");*/
}

void Component::OnTriggerExit(Node* object)
{
	//OutputDebugString("z ");
	//OutputDebugString(pOwner->GetName().c_str());
	//OutputDebugString(" wyszedl ");
	//OutputDebugString(object->GetName().c_str());
	//OutputDebugString("\n");
}

void Component::OnCollisionEnter(Node* object)
{

}

void Component::OnCollisionExit(Node* object)
{

}

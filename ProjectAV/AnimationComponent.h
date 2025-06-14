#pragma once

#include "Component.h"
#include "Animator.h"
#include <string>

class AnimationComponent : public Component
{
public:
	AnimationComponent(Node* owner, std::string tag = "", std::string animationPath = "") : Component(owner, tag)
	{
		animation = new Animation(animationPath, owner->GetComponent<ModelComponent>());
		animator = new Animator(animation);
	}
	~AnimationComponent()
	{
		delete animation;
		delete animator;
	}
	
	void Update(float dt) override
	{
		animator->UpdateAnimation(dt);
	}

private:
	Animator* animator;
	Animation* animation;
};


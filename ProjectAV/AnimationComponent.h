#pragma once

#include "Component.h"
#include "Animator.h"
#include <string>
#include "ModelCache.h" 
class AnimationComponent : public Component
{
public:
	AnimationComponent(Node* owner, std::string tag = "", std::string animationPath = "")
		: Component(owner, tag)
	{
		// Pobierz scenê z cache'a RAZ
		const aiScene* pScene = ModelCache::Get().LoadOrGet(animationPath).pScene;

		// U¿yj tej samej, wczytanej sceny do stworzenia wszystkich animacji
		for (unsigned int i = 0; i < pScene->mNumAnimations; i++)
		{
			// Przeka¿ wskaŸnik do sceny, a nie œcie¿kê do pliku
			animations.push_back(new Animation(pScene, owner->GetComponent<ModelComponent>(), i));
		}

		animator = new Animator(animations[0]);
	}
	~AnimationComponent()
	{
		for (Animation* anim : animations) {
			delete anim; 
		}

		delete animator;
	}
	
	void Update(float dt) override
	{
		animator->UpdateAnimation(dt);
	}

	void PlayAnimation(int index, float transitionDuration = 0.2f, bool loop = true)
	{
		if (animator && index >= 0 && static_cast<size_t>(index) < animations.size()) { 
			animator->PlayAnimation(animations[index], transitionDuration, loop);
		}
	}


	Animation* GetCurrentPlayingAnimationRaw() const {
		if (animator) { 
			return animator->GetCurrentAnimation();  
		}
		return nullptr;
	}
	Animation* GetAnimationByIndex(int index) const {
		if (index >= 0 && static_cast<size_t>(index) < animations.size()) {
			return animations[index];
		}
		return nullptr;
	}
	Animator* animator;
	Animation* animation;
	std::vector<Animation*> animations;
};


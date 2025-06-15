#pragma once

#include "Component.h"
#include "Animator.h"
#include <string>

class AnimationComponent : public Component
{
public:
	AnimationComponent(Node* owner, std::string tag = "", std::string animationPath = "") : Component(owner, tag)
	{
		Assimp::Importer importer;
		constexpr unsigned int kImportFlags =
			aiProcess_Triangulate |
			aiProcess_ConvertToLeftHanded |
			aiProcess_LimitBoneWeights |
			aiProcess_JoinIdenticalVertices |
			aiProcess_ImproveCacheLocality |
			aiProcess_GenSmoothNormals;
		const aiScene* scene = importer.ReadFile(animationPath, kImportFlags);
		for (int i = 0; i < scene->mNumAnimations; i++)
		{
			animations.push_back(new Animation(animationPath, owner->GetComponent<ModelComponent>(), i));
		}

		animator = new Animator(animations[0]);
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

	void PlayAnimation(int index)
	{
		animator->PlayAnimation(animations[index]);
	}

	Animator* animator;
	Animation* animation;
	std::vector<Animation*> animations;
};


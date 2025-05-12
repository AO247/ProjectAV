#pragma once
#include <string>
#include <vector>
#include "Bone.h"


struct AnimationClip {
    std::string name;
    double duration;         // Duration in ticks
    double ticksPerSecond;
    std::vector<BoneAnimation> channels; // One channel per bone that's animated
    // Root node of the animated skeleton hierarchy (from Assimp's aiNode structure)
    // aiNode* rootNode; // You'll likely convert this to your own node structure
};

class Animation
{
};


// AnimationComponent.h

#pragma once
#include "Component.h"
#include "Bone.h" // For BoneInfo (though not directly used in these structs, good context)
#include <string>
#include <vector>
#include <unordered_map>
#include <DirectXMath.h>
#include <types.h>

// Forward declarations for Assimp types (if not fully included via other headers)
struct aiNode;
struct aiScene;
struct aiAnimation; // Specifically for animation processing
struct aiNodeAnim;  // For channels within an animation

// Forward declarations for your classes
class ModelComponent;
class ModelInternalNode; // From ModelComponent.h or its own file

namespace dx = DirectX; // For DirectXMath types

// --- Animation Data Structures ---
// These can be moved to a separate "AnimationTypes.h" if they grow complex or are used elsewhere.

struct PositionKeyframe {
    float time;          // The time of this keyframe in ticks
    dx::XMFLOAT3 value;  // The position value at this keyframe

    // Constructor from Assimp's vector type
    PositionKeyframe(float t, const aiVector3D& v) : time(t), value(v.x, v.y, v.z) {}
};

struct RotationKeyframe {
    float time;          // The time of this keyframe in ticks
    dx::XMFLOAT4 value;  // The rotation value (as a quaternion) at this keyframe

    // Constructor from Assimp's quaternion type
    RotationKeyframe(float t, const aiQuaternion& q) : time(t), value(q.x, q.y, q.z, q.w) {}
};

struct ScalingKeyframe {
    float time;          // The time of this keyframe in ticks
    dx::XMFLOAT3 value;  // The scaling value at this keyframe

    // Constructor from Assimp's vector type
    ScalingKeyframe(float t, const aiVector3D& v) : time(t), value(v.x, v.y, v.z) {}
};

// Represents an animation channel, which affects a single node/bone
struct AnimationChannel {
    std::string nodeName; // Name of the Assimp node (bone) this channel targets
    std::vector<PositionKeyframe> positionKeys;
    std::vector<RotationKeyframe> rotationKeys;
    std::vector<ScalingKeyframe>  scalingKeys;
};

// Represents a single animation sequence (e.g., "Walk", "Run", "Idle")
struct Animation {
    std::string name;
    float durationTicks;    // Duration of the animation in ticks (as defined in the file)
    float ticksPerSecond;   // How many ticks occur per second (frames per second essentially)
    std::vector<AnimationChannel> channels; // All channels (bone animations) in this sequence

    // Optional: Map node name to channel index for faster lookup during playback
    // This is useful if you have many channels and frequently need to find one by name.
    std::unordered_map<std::string, size_t> channelMap;

    // Helper to get duration in seconds
    float GetDurationSeconds() const {
        return (ticksPerSecond > 0.00001f) ? (durationTicks / ticksPerSecond) : 0.0f;
    }
};
// --- End Animation Data Structures ---


class AnimationComponent : public Component
{
public:
    AnimationComponent(Node* owner);
    ~AnimationComponent() override = default;

    void Initialize(); // Call AFTER ModelComponent is initialized
    void Update(float dt) override;

    // Method to load animations from a file (often the same model file)
    bool LoadAnimationsFromFile(const std::string& filePath, const std::string& modelFilePathForSkeleton); // modelFilePathForSkeleton is needed to ensure skeleton compatibility

    // Placeholder for ImGui
    void ShowAnimationControls();

    // More methods will be added later (Play, Pause, GetBoneTransforms, etc.)

private:
    // Helper to parse aiAnimation structures from an aiScene
    void ProcessAnimations(const aiScene* scene);

    ModelComponent* mCachedModelComponent = nullptr; // To get skeleton info if needed

    // Storage for all loaded animations
    std::unordered_map<std::string, Animation> mAnimations;

    // State for current animation playback (will be used later)
    std::string mCurrentAnimationName;
    float mCurrentTimeTicks = 0.0f;
    float mAnimationSpeed = 1.0f;
    bool mIsPlaying = false;
    bool mLoop = true;

    std::vector<dx::XMFLOAT4X4> mFinalBoneMatrices; // Final transforms for shader
    // Moved MAX_SHADER_BONES from here to be a global const or part of a config if shared
    // For now, AnimationComponent can define its own limit if needed.
    static const int MAX_ANIM_COMPONENT_BONES = 128; // Max bones this component will manage matrices for (can match shader)
};
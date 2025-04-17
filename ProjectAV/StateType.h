#pragma once

// Defines the possible high-level states for AI entities.
// Keep this simple and focused on distinct behaviors.
enum class StateType
{
    NONE,
    IDLE,
    FOLLOW,
	ATTACK,
    STAY
};
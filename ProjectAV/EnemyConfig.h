#pragma once

enum class EnemyType {
	BASIC,
	FLYING,
	RANGED,
	MAGE,
	TANK,
	EXPLOSIVE,
	FRENZY,
	NONE,
	COUNT
};

namespace EnemyAnimationIndices {
	constexpr int BASIC_ATTACK1 = 0;
	constexpr int BASIC_ATTACK2 = 1;
	constexpr int BASIC_WALK = 3;

	constexpr int FLYING_ATTACK = 1;
	constexpr int FLYING_FLYING = 3;

	constexpr int RANGED_ATTACK1 = 0;
	constexpr int RANGED_ATTACK2 = 1;
	constexpr int RANGED_WALK = 3;

	constexpr int MAGE_ATTACK1 = 3;
	constexpr int MAGE_ATTACK2 = 4;
	constexpr int MAGE_ATTACK3 = 5;
	constexpr int MAGE_ATTACK4 = 2;

	constexpr int EXPLOSIVE_WALK = 10;
	constexpr int EXPLOSIVE_JUMP = 7;

	constexpr int TANK_ATTACK1 = 0;
	constexpr int TANK_ATTACK2 = 1;
	constexpr int TANK_POSE = 2;
	constexpr int TANK_WALK = 4;

}
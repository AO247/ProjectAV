#pragma once

enum class EnemyType {
	BASIC,

	COUNT
};

namespace EnemyAnimationIndices {
	constexpr int BASIC_ATTACK1 = 0;
	constexpr int BASIC_ATTACK2 = 1;
	constexpr int BASIC_WALK = 3;
}
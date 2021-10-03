#pragma once

#include "melee/player.h"
#include "util/vector.h"
#include <cmath>

bool check_ucf_xsmash(const Player *player);

inline int abs_coord_to_int(float x)
{
	// Small bias converts Nana coords back to the corresponding Popo coords
	return static_cast<int>(std::abs(x) * 80 - .0001f) + 1;
}

inline int coord_to_int(float x)
{
	return std::copysign(abs_coord_to_int(x), x);
}

inline bool is_rim_coord(const vec2 &coords)
{
	// Note: Emulates a bug with UCF's rim behavior that incorrectly considers (5875, 7875) in
	// any quadrant a rim coord. The proper check would be > 80 * 80.
	const auto converted = vec2i(abs_coord_to_int(coords.x) + 1,
	                             abs_coord_to_int(coords.y) + 1);
	return converted.length_sqr() >= 80 * 80;
}
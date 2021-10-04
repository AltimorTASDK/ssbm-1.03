#pragma once

#include "melee/player.h"
#include "util/vector.h"
#include <cmath>

// 0.2750
constexpr auto deadzone = 22;

bool check_ucf_xsmash(const Player *player);

inline int abs_coord_to_int(float x)
{
	// Small bias converts Nana coords back to the corresponding Popo coords
	return static_cast<int>(std::abs(x) * 80 - .0001f) + 1;
}

inline bool is_rim_coord(const vec2 &coords)
{
	const auto converted = vec2i(abs_coord_to_int(coords.x) + 1,
	                             abs_coord_to_int(coords.y) + 1);
	return converted.length_sqr() > 80 * 80;
}
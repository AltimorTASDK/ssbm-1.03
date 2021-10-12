#pragma once

#include <array>
#include "melee/rules.h"

/*constexpr auto ledge_grab_limit_values = std::array {
	0, 30, 35, 40, 45, 50, 55, 60
};*/

constexpr auto ledge_grab_limit_values = std::array {
	0, 1, 30, 35, 40, 45, 50, 55, 60
};

constexpr auto air_time_limit_values = std::array {
	0, 120, 150, 180, 210, 240, 270, 300, 330, 360
};

inline int get_ledge_grab_limit()
{
	const auto *rules = GetGameRules();
	return ledge_grab_limit_values[rules->handicap];
}

inline int get_air_time_limit()
{
	const auto *rules = GetGameRules();
	return air_time_limit_values[rules->damage_ratio];
}
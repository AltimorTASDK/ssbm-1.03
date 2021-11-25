#pragma once

#include <array>
#include "melee/rules.h"
#include "melee/stage.h"

constexpr auto ledge_grab_limit_values = std::array {
	0, 30, 35, 40, 45, 50, 55, 60
};

constexpr auto air_time_limit_values = std::array {
	0, 120, 150, 180, 210, 240, 270, 300, 330, 360
};

bool get_settings_lock();

inline int get_ledge_grab_limit()
{
	const auto *rules = GetGameRules();
	return ledge_grab_limit_values[rules->ledge_grab_limit];
}

inline int get_air_time_limit()
{
	const auto *rules = GetGameRules();
	return air_time_limit_values[rules->air_time_limit];
}

inline ucf_type get_ucf_type()
{
	const auto *rules = GetGameRules();
	return rules->controller_fix;
}

inline bool is_widescreen()
{
	const auto *rules = GetGameRules();
	return rules->widescreen;
}

inline bool should_use_oss()
{
	const auto *rules = GetGameRules();
	return rules->stage_mods == stage_mod_type::oss;
}

inline bool is_stage_frozen(u8 id)
{
	const auto *rules = GetGameRules();

	switch (id) {
	case Stage_PS:
		if (rules->stage_mods == stage_mod_type::ps)
			return true;
	case Stage_FoD:
	case Stage_YS:
	case Stage_DL:
	case Stage_FD:
		if (rules->stage_mods == stage_mod_type::all)
			return true;
	default:
		return false;
	}
}

inline latency_mode get_latency()
{
	const auto *rules = GetGameRules();
	return rules->latency;
}
#pragma once

#include "compat/20XX.h"
#include "melee/match.h"
#include "melee/rules.h"
#include "melee/stage.h"
#include <array>

constexpr auto ledge_grab_limit_values = std::array {
	0, 30, 35, 40, 45, 50, 55, 60
};

constexpr auto air_time_limit_values = std::array {
	0, 120, 150, 180, 210, 240, 270, 300, 330, 360
};

bool get_settings_lock();

inline bool is_match_crew(const StartMeleeRules &rules)
{
	// Check whether the currently running match is crew and not 1p mode
	return GetGameRules()->mode == Mode_Crew && rules.is_stock_match;
}

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

inline latency_mode get_latency()
{
	const auto *rules = GetGameRules();
	return rules->latency;
}

inline controls_type get_controls()
{
	const auto *rules = GetGameRules();
	return rules->controls;
}

inline bool should_use_oss()
{
	if (is_20XX_stage_select())
		return true;

	const auto *rules = GetGameRules();
	return rules->stage_mods == stage_mod_type::oss;
}

inline bool is_stage_frozen(int id)
{
	const auto *rules = GetGameRules();

	switch (id) {
	case Stage_FD:
	case Stage_PS:
		if (rules->stage_mods == stage_mod_type::fdps)
			return true;
	case Stage_FoD:
	case Stage_YS:
	case Stage_DL:
		if (rules->stage_mods == stage_mod_type::all)
			return true;
	default:
		return false;
	}
}
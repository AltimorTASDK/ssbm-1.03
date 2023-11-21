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
#if defined(LGL_ROTATOR)
	return ledge_grab_limit_values[rules->ledge_grab_limit];
#elif defined(LGL_LEGACY)
	switch (rules->stock_time_limit) {
		case 8:  return 50;
		case 7:  return 45;
		case 6:  return 40;
		default: return 0;
	}
#else
	return rules->stock_time_limit * 5;
#endif
}

inline int get_air_time_limit()
{
#ifdef ATL_ROTATOR
	const auto *rules = GetGameRules();
	return air_time_limit_values[rules->air_time_limit];
#else
	return 0;
#endif
}

inline ucf_type get_ucf_type()
{
	const auto *rules = GetGameRules();
	return rules->controller_fix;
}

inline bool is_widescreen()
{
	const auto *rules = GetGameRules();
	return rules->widescreen == widescreen_mode::on;
}

inline bool is_widescreen_crop()
{
	const auto *rules = GetGameRules();
	return rules->widescreen == widescreen_mode::crop;
}

inline latency_mode get_latency()
{
	const auto *rules = GetGameRules();
	return rules->latency;
}

inline controls_type get_controls()
{
#ifndef TOURNAMENT
	const auto *rules = GetGameRules();
	return rules->controls;
#else
	return controls_type::z_jump;
#endif
}

inline bool should_use_oss()
{
	if (is_20XX_stage_select())
		return true;

#ifdef SSS_ROTATOR
	const auto *rules = GetGameRules();
	return rules->stage_mods == sss_type::oss;
#else
	return false;
#endif
}

inline bool is_stage_frozen(int id)
{
	if (is_20XX_stage_select())
		return false;

#ifdef SSS_ROTATOR
#ifdef FULL_SSS_ROTATOR
	const auto *rules = GetGameRules();
#endif

	switch (id) {
	case Stage_FD:
	case Stage_PS:
#ifdef FULL_SSS_ROTATOR
		if (rules->stage_mods == sss_type::fdps)
			return true;
#endif
	case Stage_FoD:
	case Stage_YS:
	case Stage_DL:
#ifdef FULL_SSS_ROTATOR
		if (rules->stage_mods == sss_type::all)
#endif
			return true;
	default:
		return false;
	}
#else
	return true;
#endif
}
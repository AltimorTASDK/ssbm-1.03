#pragma once

#include "melee/music.h"
#include "melee/rules.h"
#include "util/gc/memcard.h"
#include <gctypes.h>

enum class config_version : u8 {
	a1 = 0,
	a2 = 1,
	a3 = 2,
	max,
	current = max - 1
};

template<config_version>
struct config_values;

template<>
struct [[gnu::packed]] config_values<config_version::a1> {
	u8 widescreen        = false;
	latency_mode latency = latency_mode::crt;
	int stage_bgm[6]     = { BGM_Undefined, BGM_Undefined, BGM_Undefined,
	                         BGM_Undefined, BGM_Undefined, BGM_Undefined };
	int menu_bgm         = BGM_Undefined;
};

template<>
struct [[gnu::packed]] config_values<config_version::a2> : config_values<config_version::a1> {
	// Added in A2
	config_version version  = config_version::current;
	u8 ledge_grab_limit     = 0;
	u8 air_time_limit       = 0;
	u8 stock_time_limit     = 8;
	ucf_type controller_fix = ucf_type::hax;
	sss_type stage_mods     = sss_type::all;
};

template<>
struct [[gnu::packed]] config_values<config_version::a3> : config_values<config_version::a2> {
	// Added in A3/B3
#ifdef TOURNAMENT
	controls_type controls = controls_type::z_jump;
#else
	controls_type controls = controls_type::all;
#endif
};

struct [[gnu::packed]] saved_config : config_values<config_version::current> {
	saved_config();
	void load();
	void save();
};

inline card_write_buffer<saved_config> config_buffer;

inline saved_config &config = *config_buffer;
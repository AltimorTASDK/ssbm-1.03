#pragma once

#include "melee/rules.h"
#include <gctypes.h>

struct [[gnu::packed]] saved_config {
	u8 widescreen = false;
	latency_mode latency = latency_mode::crt;
	int stage_bgm[6] = { -1, -1, -1, -1, -1, -1 };
	int menu_bgm = -1;
	
	saved_config();
	
	void save();
};

inline saved_config config;
#pragma once

#include <gctypes.h>

enum class cstick_type {
	smash,
	tilt
};

struct controller_config {
	u8 z_jump_bit;
	bool perfect_wavedash;
	cstick_type c_up;
	cstick_type c_horizontal;
	cstick_type c_down;
	bool tap_jump;
};

inline controller_config controller_configs[4];
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
	
	controller_config()
	{
		reset();
	}
	
	void reset()
	{
		z_jump_bit = 0;
		perfect_wavedash = false;
		c_up = cstick_type::smash;
		c_horizontal = cstick_type::smash;
		c_down = cstick_type::smash;
		tap_jump = true;
	}
};

inline controller_config controller_configs[4];
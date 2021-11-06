#pragma once

#include "melee/player.h"
#include <gctypes.h>

enum class cstick_type {
	smash,
	tilt
};

struct controller_config {
	static controller_config defaults;
	
	u8 z_jump_bit             = 0;
	bool perfect_wavedash     = false;
	cstick_type c_up          = cstick_type::smash;
	cstick_type c_horizontal  = cstick_type::smash;
	cstick_type c_down        = cstick_type::smash;
	bool tap_jump             = true;
	
	void reset()
	{
		*this = defaults;
	}
};

inline controller_config controller_config::defaults = {};

inline controller_config controller_configs[4];

inline const controller_config &get_player_config(const Player *player)
{
	if (!Player_IsCPU(player))
		return controller_configs[player->port];
	else
		return controller_config::defaults;
}
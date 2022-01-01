#pragma once

#include "melee/player.h"
#include "melee/rules.h"
#include "rules/values.h"
#include <gctypes.h>

enum class cstick_type {
	smash,
	tilt
};

struct controller_config {
	static const controller_config defaults;

	u8 z_jump_bit            = 0;
	bool perfect_angles      = false;
	cstick_type c_up         = cstick_type::smash;
	cstick_type c_horizontal = cstick_type::smash;
	cstick_type c_down       = cstick_type::smash;
	bool tap_jump            = true;

	void reset()
	{
		*this = defaults;
	}

	void make_legal()
	{
		switch (get_controls()) {
		case controls_type::z_angles:
			c_up             = defaults.c_up;
			c_horizontal     = defaults.c_horizontal;
			c_down           = defaults.c_down;
			tap_jump         = defaults.tap_jump;
		case controls_type::z_jump:
			perfect_angles   = defaults.perfect_angles;
		}
	}

	bool is_illegal() const
	{
		return perfect_angles
		    || c_up         != cstick_type::smash
		    || c_horizontal != cstick_type::smash
		    || c_down       != cstick_type::smash
		    || !tap_jump;
	}
};

inline const controller_config controller_config::defaults = {};

inline controller_config controller_configs[4];

inline const controller_config &get_player_config(const Player *player)
{
	if (!Player_IsCPUControlled(player))
		return controller_configs[Player_GetPort(player)];
	else
		return controller_config::defaults;
}
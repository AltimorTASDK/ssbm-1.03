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
#ifndef TOURNAMENT
	bool perfect_angles      = false;
	cstick_type c_up         = cstick_type::smash;
	cstick_type c_horizontal = cstick_type::smash;
	cstick_type c_down       = cstick_type::smash;
	bool tap_jump            = true;
#endif

	void reset(int port)
	{
		// Disable rumble on unplug
		SetPortRumbleFlag(port, false);
		*this = defaults;
	}

	void make_legal()
	{
#ifndef TOURNAMENT
		switch (get_controls()) {
		case controls_type::no_angles:
			perfect_angles   = defaults.perfect_angles;
			break;
		case controls_type::z_jump:
			perfect_angles   = defaults.perfect_angles;
		case controls_type::z_angles:
			c_up             = defaults.c_up;
			c_horizontal     = defaults.c_horizontal;
			c_down           = defaults.c_down;
			tap_jump         = defaults.tap_jump;
		}
#endif
	}

#ifdef OLD_CSS_TOGGLES
	bool is_illegal() const
	{
#ifndef TOURNAMENT
		return perfect_angles
		    || c_up         != cstick_type::smash
		    || c_horizontal != cstick_type::smash
		    || c_down       != cstick_type::smash
		    || !tap_jump;
#else
		return false;
#endif
	}
#endif
};

inline const controller_config controller_config::defaults;

inline controller_config controller_configs[4];

inline const controller_config &get_player_config(const Player *player)
{
	if (!Player_IsCPUControlled(player))
		return controller_configs[Player_GetPort(player)];
	else
		return controller_config::defaults;
}
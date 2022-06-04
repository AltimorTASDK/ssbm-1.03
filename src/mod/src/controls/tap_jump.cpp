#ifndef TOURNAMENT

#include "controls/config.h"
#include "melee/player.h"

extern "C" bool is_tap_jump_on(const Player *player)
{
	return get_player_config(player).tap_jump;
}

#endif
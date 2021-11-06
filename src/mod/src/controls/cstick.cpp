#include "controls/config.h"
#include "hsd/pad.h"
#include "melee/constants.h"
#include "melee/player.h"

extern "C" bool orig_Player_CheckCStickUSmashInput(const Player *player);
extern "C" bool hook_Player_CheckCStickUSmashInput(const Player *player)
{
	if (get_player_config(player).c_up != cstick_type::smash)
		return false;
		
	return orig_Player_CheckCStickUSmashInput(player);
}

extern "C" bool orig_Player_CheckCStickFSmashInput(const Player *player);
extern "C" bool hook_Player_CheckCStickFSmashInput(const Player *player)
{
	if (get_player_config(player).c_horizontal != cstick_type::smash)
		return false;
		
	return orig_Player_CheckCStickFSmashInput(player);
}

extern "C" bool orig_Player_CheckCStickDSmashInput(const Player *player);
extern "C" bool hook_Player_CheckCStickDSmashInput(const Player *player)
{
	if (get_player_config(player).c_down != cstick_type::smash)
		return false;
		
	return orig_Player_CheckCStickDSmashInput(player);
}

extern "C" bool check_cstick_utilt(const Player *player)
{
	if (get_player_config(player).c_up != cstick_type::tilt)
		return false;
		
	return player->input.cstick.y      >= plco->usmash_threshold &&
	       player->input.last_cstick.y <  plco->usmash_threshold;
}

extern "C" bool check_cstick_ftilt(const Player *player)
{
	if (get_player_config(player).c_horizontal != cstick_type::tilt)
		return false;
		
	return player->input.cstick.x      * player->direction >= plco->x_smash_threshold &&
	       player->input.last_cstick.x * player->direction <  plco->x_smash_threshold;
}

extern "C" bool check_cstick_dtilt(const Player *player)
{
	if (get_player_config(player).c_down != cstick_type::tilt)
		return false;
		
	return player->input.cstick.y      <= plco->dsmash_threshold &&
	       player->input.last_cstick.y >  plco->dsmash_threshold;
}

extern "C" bool check_cstick_jab(const Player *player)
{
	if (get_player_config(player).c_horizontal != cstick_type::tilt)
		return false;
		
	return player->input.cstick.x      * -player->direction >= plco->x_smash_threshold &&
	       player->input.last_cstick.x * -player->direction <  plco->x_smash_threshold;
}

extern "C" bool check_cstick_rapid_jab(const Player *player)
{
	if (get_player_config(player).c_horizontal != cstick_type::tilt)
		return false;
		
	return (player->input.cstick.x      * -player->direction >= plco->x_smash_threshold) !=
	       (player->input.last_cstick.x * -player->direction >= plco->x_smash_threshold);
}
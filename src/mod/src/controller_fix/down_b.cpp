#include "dolphin/pad.h"
#include "melee/constants.h"
#include "melee/player.h"
#include "rules/values.h"

extern "C" bool orig_Player_CheckDownBInput(Player *player);
extern "C" bool hook_Player_CheckDownBInput(Player *player)
{
	if (!(player->input.instant_buttons & Button_B))
		return false;

	if (get_cfix() < cfix::c)
		return player->input.stick.y < -plco->y_special_threshold;
	else
		return player->input.stick.y <= -plco->y_special_threshold;
}
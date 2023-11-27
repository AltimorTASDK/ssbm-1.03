#include "dolphin/pad.h"
#include "melee/action_state.h"
#include "melee/constants.h"
#include "melee/player.h"
#include "rules/values.h"

extern "C" bool orig_Player_CheckSideBInput(Player *player);
extern "C" bool hook_Player_CheckSideBInput(Player *player)
{
	if (!(player->input.instant_buttons & Button_B))
		return false;

	if (get_cfix() >= cfix::c) {
		// Make neutral/down B adjacent to the Run region
		switch (player->action_state) {
		case AS_Dash:
		case AS_Run:
			return std::abs(player->input.stick.x) >= plco->run_threshold;
		}
	}

	return std::abs(player->input.stick.x) >= plco->x_special_threshold;
}
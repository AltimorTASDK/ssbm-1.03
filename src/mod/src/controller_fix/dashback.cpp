#include "melee/action_state.h"
#include "melee/constants.h"
#include "melee/player.h"
#include "util/melee/pad.h"

static void check_dashback(HSD_GObj *gobj)
{
	auto *player = gobj->get<Player>();

	if (Player_IsCPU(player))
		return;

	// Must be tilt turn f2
	if (player->animation_frame != 2.f)
		return;

	if (player->as_data.Turn.turn_finished)
		return;

	// Must satisfy vanilla xsmash conditions
	// UCF v0.74+ doesn't check direction and it allows a UCF only ICs desync
	if (player->input.stick.x * -player->direction < plco->x_smash_threshold)
		return;

	if (player->input.stick_x_hold_time >= 2)
		return;

	// Hardware value comparison
	if (!check_ucf_xsmash(player))
		return;

	player->direction = -player->direction;
	player->as_data.Turn.turn_finished = true;
	player->as_data.Turn.can_dash = true;

	if (player->character_id != CID_Popo)
		return;

	// Retroactively write dashback to Nana inputs
	auto *nana_gobj = PlayerBlock_GetSubCharGObj(player->slot, 1);
	if (nana_gobj == nullptr)
		return;

	auto *nana = nana_gobj->get<Player>();
	nana->popo_data_write->direction = player->direction;
	nana->popo_data_write->stick = popo_to_nana(player->input.stick);
}

extern "C" void orig_Interrupt_AS_Turn(HSD_GObj *gobj);
extern "C" void hook_Interrupt_AS_Turn(HSD_GObj *gobj)
{
	check_dashback(gobj);
	orig_Interrupt_AS_Turn(gobj);
}
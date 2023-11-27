#include "melee/action_state.h"
#include "melee/constants.h"
#include "melee/player.h"
#include "rules/values.h"
#include "util/melee/pad.h"

bool should_suppress_squatrv(const HSD_GObj *gobj)
{
	if (get_cfix() < cfix::c)
		return false;

	const auto *player = gobj->get<Player>();

	if (Player_IsCPUControlled(player))
		return false;

	// Must be rim coord (quarter circle motion)
	if (!is_rim_coord(player->input.stick))
		return false;

	// Raise the max SquatWait coord by 2 values.
	// This would be 6000, but 5900 avoids an ICs desync.
	if (player->input.stick.y > -.5900f)
		return false;

	return true;
}

extern "C" bool orig_Interrupt_SquatRv(HSD_GObj *gobj);
extern "C" bool hook_Interrupt_SquatRv(HSD_GObj *gobj)
{
	return !should_suppress_squatrv(gobj) && orig_Interrupt_SquatRv(gobj);
}

extern "C" bool orig_Interrupt_TurnOrDash(HSD_GObj *gobj);
extern "C" bool hook_Interrupt_TurnOrDash(HSD_GObj *gobj)
{
	if (orig_Interrupt_TurnOrDash(gobj))
		return true;

	if (get_cfix() < cfix::d)
		return false;

	auto *player = gobj->get<Player>();

	if (Player_IsCPUControlled(player))
		return false;

	// DOOC only
	if (player->action_state != AS_SquatWait)
		return false;

	// Check xsmash with 3f window
	if (player->input.stick_x_hold_time >= 3)
		return false;

	const auto forward_x = player->input.stick.x * player->direction;

	if (forward_x >= plco->x_smash_threshold)
		AS_020_Dash(gobj, true);
	else if (-forward_x >= plco->x_smash_threshold)
		AS_018_SmashTurn(gobj);
	else
		return false;

	return true;
}
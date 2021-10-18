#include "melee/action_state.h"
#include "melee/constants.h"
#include "melee/player.h"
#include "util/melee/pad.h"

bool should_suppress_squatrv(const HSD_GObj *gobj)
{
	const auto *player = gobj->get<Player>();

	// Must be outside deadzone for exactly 1f (intending to dash on next frame)
	if (player->input.stick.x == 0.f)
		return false;

#ifndef UCF
	// Extend window to 2f to match 3f dbooc
	if (player->input.stick_x_hold_time >= 2)
		return false;
#else
	if (player->input.stick_x_hold_time >= 1)
		return false;
#endif
		
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

#ifndef UCF
	auto *player = gobj->get<Player>();

	// DBOOC only
	if (player->action_state != AS_SquatWait)
		return false;
		
	// Check xsmash back with 3f window
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
#else
	return false;
#endif
}
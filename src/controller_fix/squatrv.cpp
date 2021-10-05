#include "melee/player.h"
#include "util/coords.h"

bool should_suppress_squatrv(const HSD_GObj *gobj)
{
	const auto *player = gobj->get<Player>();

	// Must be outside deadzone for exactly 1f (intending to dash on next frame)
	if (player->input.stick.x == 0.f)
		return false;

	if (player->input.stick_x_hold_time >= 1)
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

extern "C" int orig_Interrupt_SquatRv(HSD_GObj *gobj);
extern "C" int hook_Interrupt_SquatRv(HSD_GObj *gobj)
{
	return !should_suppress_squatrv(gobj) && orig_Interrupt_SquatRv(gobj);
}
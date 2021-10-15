#include "os/os.h"
#include "melee/constants.h"
#include "melee/player.h"
#include "util/melee/pad.h"

static bool should_suppress_spotdodge(HSD_GObj *gobj)
{
	const auto *player = gobj->get<Player>();

	// Always prioritize cstick
	if (player->input.cstick.y <= plco->spot_dodge_stick_threshold)
		return false;

	// Must be on a platform (not checked by UCF)
	if (!Physics_IsOnPlatform(&player->phys))
		return false;

	// Roll must be disabled
	if (player->input.stick_x_hold_time < plco->roll_stick_frames)
		return false;

#ifdef UCF
	// Must be above Y -8000
	if (player->input.stick.y <= -.8000f)
		return false;
#else
	// Must be outside of X deadzone
	if (player->input.stick.x == 0.f)
		return false;
#endif

	// Must be a rim coord
	if (!is_rim_coord(player->input.stick))
		return false;

	return true;
}

extern "C" bool orig_Interrupt_SpotDodge_OoS(HSD_GObj *gobj);
extern "C" bool hook_Interrupt_SpotDodge_OoS(HSD_GObj *gobj)
{
	return !should_suppress_spotdodge(gobj) && orig_Interrupt_SpotDodge_OoS(gobj);
}

extern "C" bool orig_Interrupt_SpotDodge(HSD_GObj *gobj);
extern "C" bool hook_Interrupt_SpotDodge(HSD_GObj *gobj)
{
	return !should_suppress_spotdodge(gobj) && orig_Interrupt_SpotDodge(gobj);
}
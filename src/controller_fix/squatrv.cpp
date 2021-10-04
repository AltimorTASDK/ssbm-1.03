#include "melee/action_state.h"
#include "melee/constants.h"
#include "melee/player.h"
#include <gctypes.h>

extern "C" bool orig_Interrupt_SquatRv(HSD_GObj *gobj);
extern "C" bool hook_Interrupt_SquatRv(HSD_GObj *gobj)
{
	// Raise SquatRv threshold by 2 values. This would be 6000, but 5900 avoids an ICs desync.
	const auto *player = gobj->get<Player>();
	if (player->input.stick.y <= -.5900f)
		return false;
		
	AS_041_SquatRv(gobj);
	return true;
}
#if defined(NTSC100) || defined(NTSC101)

#include "melee/action_state.h"
#include "melee/player.h"
#include <gctypes.h>

extern "C" void hook_Collision_AS_Bowser_FlameStartAir(HSD_GObj *gobj)
{
	if (!Player_CollisionAir(gobj))
		return;

	// v1.02 logic
	auto *player = gobj->get<Player>();
	Player_AirToGroundTransition(player);
	Player_ASChange(gobj, 0x155, 0xC4C5080, nullptr, player->animation_frame, 1.f, 0.f);
}

#endif
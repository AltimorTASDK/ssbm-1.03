#include "hsd/gobj.h"
#include "melee/action_state.h"
#include "melee/constants.h"
#include "melee/object.h"
#include "melee/player.h"
#include "rules/values.h"
#include "util/melee/pad.h"
#include <gctypes.h>

// Custom AS data
struct doraki_data {
	// Whether a ledgefall was performed this frame
	bool ledgefall;
	// Whether the player was eligible to doraki on f1 of Fall
	bool can_doraki;
	// Position/ECB flags when player was originally eligible to doraki
	vec3 doraki_position;
	u32 doraki_ecb_flags;
};

extern "C" void orig_AS_029_Fall(HSD_GObj *gobj);
extern "C" void hook_AS_029_Fall(HSD_GObj *gobj)
{
	if (get_ucf_type() != ucf_type::ucf) {
		// Record whether Fall was entered from CliffWait
		auto *player = gobj->get<Player>();
		auto *as_data = player->custom_as_data<doraki_data>();
		as_data->ledgefall = player->action_state == AS_CliffWait;
		as_data->can_doraki = false;
	}

	orig_AS_029_Fall(gobj);
}

static bool check_doraki_input(const Player *player)
{
	// Must be pressing away from wall
	if (player->input.stick.x * -player->walljump_direction < plco->walljump_stick_threshold)
		return false;

	if (player->input.stick_x_hold_time >= 2)
		return false;

	// Hardware value comparison
	return check_ucf_xsmash(player);
}

extern "C" bool orig_Interrupt_Walljump(HSD_GObj *gobj);
extern "C" bool hook_Interrupt_Walljump(HSD_GObj *gobj)
{
	constexpr auto wall_hug_mask = Collide_LeftWallHug | Collide_RightWallHug;
	auto *player = gobj->get<Player>();
	auto *as_data = player->custom_as_data<doraki_data>();

	if (player->action_state != AS_Fall || get_ucf_type() == ucf_type::ucf)
		return orig_Interrupt_Walljump(gobj);

	if (as_data->can_doraki) {
		as_data->can_doraki = false;

		if ((player->phys.collide & wall_hug_mask) == 0 && check_doraki_input(player)) {
			// The player missed the f1 smash input and drifted away, allow an f2 doraki
			const auto position = player->position;
			const auto collide = player->phys.collide;
			const auto ecb = player->phys.ecb;
			const auto ecb_flags = player->phys.ecb_flags;
			player->position = as_data->doraki_position;
			player->phys.position = player->position;
			player->phys.collide = player->phys.last_collide;
			player->phys.ecb = player->phys.last_ecb;
			player->phys.ecb_flags = as_data->doraki_ecb_flags;

			if (orig_Interrupt_Walljump(player->gobj))
				return true;

			// Walljump failed, restore position/collide/ECB
			player->position = position;
			player->phys.position = position;
			player->phys.collide = collide;
			player->phys.ecb = ecb;
			player->phys.ecb_flags = ecb_flags;
			return false;
		}
	}

	if (orig_Interrupt_Walljump(gobj))
		return true;

	if (as_data->ledgefall) {
		// Store doraki eligibility for f2
		as_data->ledgefall = false;
		as_data->can_doraki = player->walljump_eligible_frames < 0xFE;
		if (as_data->can_doraki) {
			as_data->doraki_position = player->position;
			as_data->doraki_ecb_flags = player->phys.ecb_flags;
		}
	}

	return false;
}
#include "hsd/gobj.h"
#include "melee/action_state.h"
#include "melee/constants.h"
#include "melee/object.h"
#include "melee/player.h"
#include "rules/values.h"
#include "util/melee/pad.h"
#include <gctypes.h>

// Custom AS data
struct ledge_data {
	struct {
		// Whether to use the 50deg line for aerials after a cstick ledgefall
		bool use_50d;
	} cstick;
	struct {
		// Whether a ledgefall was performed this frame
		bool ledgefall;
		// Whether the player was eligible to doraki on f1 of Fall
		bool can_doraki;
	} doraki;
};

extern "C" bool orig_Interrupt_Walljump(HSD_GObj *gobj);

extern "C" void orig_AS_029_Fall(HSD_GObj *gobj);
extern "C" void hook_AS_029_Fall(HSD_GObj *gobj)
{
	if (get_ucf_type() != ucf_type::ucf) {
		// Record whether to enable doraki and cstick ledgefall fixes
		auto *player = gobj->get<Player>();
		auto *as_data = player->custom_as_data<ledge_data>();
		const auto ledgefall = player->action_state == AS_CliffWait;
		as_data->doraki.ledgefall = ledgefall;
		as_data->doraki.can_doraki = false;
		as_data->cstick.use_50d = ledgefall && player->input.cstick != vec2::zero;
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

static bool attempt_doraki(Player *player)
{
	// The player missed the f1 smash input and drifted away, allow an f2 doraki
	const auto position = player->position;
	const auto collide = player->phys.collide;
	const auto ecb = player->phys.ecb;
	player->position = player->last_position;
	player->phys.position = player->position;
	player->phys.collide = player->phys.last_collide;
	player->phys.ecb = player->phys.last_ecb;

	if (orig_Interrupt_Walljump(player->gobj))
		return true;

	// Walljump failed, restore position/collide/ECB
	player->position = position;
	player->phys.position = position;
	player->phys.collide = collide;
	player->phys.ecb = ecb;
	return false;
}

extern "C" bool hook_Interrupt_Walljump(HSD_GObj *gobj)
{
	constexpr auto wall_hug_mask = Collide_LeftWallHug | Collide_RightWallHug;
	auto *player = gobj->get<Player>();
	auto *as_data = &player->custom_as_data<ledge_data>()->doraki;

	if (player->action_state != AS_Fall || get_ucf_type() == ucf_type::ucf)
		return orig_Interrupt_Walljump(gobj);

	if (as_data->can_doraki) {
		as_data->can_doraki = false;

		if ((player->phys.collide & wall_hug_mask) == 0 && check_doraki_input(player))
			return attempt_doraki(player);
	}

	if (orig_Interrupt_Walljump(gobj))
		return true;

	if (as_data->ledgefall) {
		// Store doraki eligibility for f2
		as_data->ledgefall = false;
		as_data->can_doraki = player->walljump_eligible_frames < 0xFE;
	}

	return false;
}

extern "C" bool orig_Interrupt_JumpAerial(HSD_GObj *gobj, bool meteor);
extern "C" bool hook_Interrupt_JumpAerial(HSD_GObj *gobj, bool meteor)
{
	auto *player = gobj->get<Player>();

	if (get_ucf_type() == ucf_type::ucf || player->action_state == AS_Fall)
		return orig_Interrupt_JumpAerial(gobj, meteor);

	if (!orig_Interrupt_JumpAerial(gobj, meteor))
		return false;

	// Initialize the 50d flag when entering JumpAerial from a state other than Fall
	player->custom_as_data<ledge_data>()->cstick.use_50d = false;
	return true;
}

extern "C" bool orig_Player_CheckCStickAerial(Player *player);
extern "C" bool hook_Player_CheckCStickAerial(Player *player)
{
	if (get_ucf_type() == ucf_type::ucf)
		return orig_Player_CheckCStickAerial(player);

	const auto state = player->action_state;

	if (state != AS_Fall && state != AS_JumpAerialF && state != AS_JumpAerialB)
		return orig_Player_CheckCStickAerial(player);

	auto *as_data = &player->custom_as_data<ledge_data>()->cstick;

	if (!as_data->use_50d)
		return orig_Player_CheckCStickAerial(player);

	const auto cstick      = player->input.cstick.abs();
	const auto last_cstick = player->input.last_cstick.abs();
	const auto angle       = get_stick_angle(cstick);
	const auto last_angle  = get_stick_angle(last_cstick);

	if (last_cstick.x < plco->aerial_threshold_x || last_angle >= plco->angle_50d) {
		if (cstick.x >= plco->aerial_threshold_x && angle < plco->angle_50d)
			return true;
	}

	if (last_cstick.y < plco->aerial_threshold_y || last_angle < plco->angle_50d) {
		if (cstick.y >= plco->aerial_threshold_y && angle >= plco->angle_50d)
			return true;
	}

	return false;
}
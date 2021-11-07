#include "melee/action_state.h"
#include "melee/constants.h"
#include "melee/player.h"
#include "rules/values.h"
#include "util/melee/pad.h"
#include <cmath>

static bool should_wiggle_out(HSD_GObj *gobj)
{
	const auto *player = gobj->get<Player>();

	if (std::abs(player->input.stick.x) < plco->tumble_wiggle_threshold)
		return false;

	// Bail out early for vanilla success
	if (player->input.stick_x_hold_time < plco->tumble_wiggle_frames)
		return true;

	// Don't apply fix for CPUs
	if (Player_IsCPUControlled(player))
		return false;

	// Must be within 2f
	if (player->input.stick_x_hold_time >= 2)
		return false;
		
	// Last frame must not have been a wiggle input i.e. don't allow buffering
	if (std::abs(player->input.last_stick.x) >= plco->tumble_wiggle_threshold)
		return false;
		
	// Hardware value comparison (UCF)
	if (get_ucf_type() == ucf_type::ucf && !check_ucf_xsmash(player))
		return false;
		
	return true;
}

// Called from tumble.S
extern "C" bool check_tumble_wiggle(HSD_GObj *gobj)
{
	if (!should_wiggle_out(gobj))
		return false;
		
	AS_029_Fall(gobj);
	return true;
}
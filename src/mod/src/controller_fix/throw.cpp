#include "melee/action_state.h"
#include "melee/constants.h"
#include "melee/player.h"
#include "rules/values.h"
#include "util/melee/pad.h"
#include <cmath>

static bool check_fthrow(const vec2 &stick, float angle)
{
	return stick.x >= plco->ftilt_threshold && angle < plco->angle_50d;
}

static bool check_bthrow(const vec2 &stick, float angle)
{
	return stick.x <= -plco->ftilt_threshold && angle < plco->angle_50d;
}

static bool check_uthrow(const vec2 &stick, float angle)
{
	return stick.y >= plco->utilt_threshold && angle >= plco->angle_50d;
}

static bool check_dthrow(const vec2 &stick, float angle)
{
	return stick.y <= plco->dtilt_threshold && angle >= plco->angle_50d;
}

static float get_stick_angle(const vec2 &stick)
{
	return std::atan2(std::abs(stick.y), std::abs(stick.x));
}

static int get_throw_state(const Player *player)
{
	const auto stick        = player->input.stick       * vec2(player->direction, 1);
	const auto last_stick   = player->input.last_stick  * vec2(player->direction, 1);
	const auto cstick       = player->input.cstick      * vec2(player->direction, 1);
	const auto last_cstick  = player->input.last_cstick * vec2(player->direction, 1);

	const auto angle        = get_stick_angle(stick);
	const auto last_angle   = get_stick_angle(last_stick);
	const auto c_angle      = get_stick_angle(cstick);
	const auto last_c_angle = get_stick_angle(last_cstick);

	// fthrow > bthrow > c-fthrow > c-bthrow > uthrow > c-uthrow > dthrow > c-dthrow

	if (check_fthrow(stick, angle) && !check_fthrow(last_stick, last_angle))
		return AS_ThrowF;

	if (check_bthrow(stick, angle) && !check_bthrow(last_stick, last_angle))
		return AS_ThrowB;

	if (check_fthrow(cstick, c_angle) && !check_fthrow(last_cstick, last_c_angle))
		return AS_ThrowF;

	if (check_bthrow(cstick, c_angle) && !check_bthrow(last_cstick, last_c_angle))
		return AS_ThrowB;

	if (check_uthrow(stick, angle) && !check_uthrow(last_stick, last_angle))
		return AS_ThrowHi;

	if (check_uthrow(cstick, c_angle) && !check_uthrow(last_cstick, last_c_angle))
		return AS_ThrowHi;

	if (check_dthrow(stick, angle) && !check_dthrow(last_stick, last_angle))
		return AS_ThrowLw;

	// dthrow buffering bug
	if (check_dthrow(cstick, c_angle) && check_dthrow(last_cstick, last_c_angle))
		return AS_ThrowLw;

	return -1;
}

extern "C" void orig_Interrupt_Throw(HSD_GObj *gobj);
extern "C" void hook_Interrupt_Throw(HSD_GObj *gobj)
{
	if (get_ucf_type() == ucf_type::ucf)
		return orig_Interrupt_Throw(gobj);

	const auto *player = gobj->get<Player>();

	// Disable for ICs to avoid breaking desync tech
	if (player->character_id == CID_Popo || player->character_id == CID_Nana)
		return orig_Interrupt_Throw(gobj);

	// Check for throws with 50d line, try old logic if no throw with 50d
	if (const auto state = get_throw_state(player); state != -1)
		Player_DoThrow(gobj, state);
	else
		orig_Interrupt_Throw(gobj);
}
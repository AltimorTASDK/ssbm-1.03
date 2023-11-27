#include "hsd/pad.h"
#include "melee/action_state.h"
#include "melee/constants.h"
#include "melee/player.h"
#include "melee/characters/dk.h"
#include "rules/values.h"
#include "util/melee/pad.h"

struct checker {
	const vec2 stick;
	const float direction;
	const bool vanilla;
	const bool vert = vanilla || get_stick_angle_abs(stick) >= plco->angle_50d;
	const bool horz = vanilla || !vert;

	bool fthrow() const { return stick.x * direction >=  plco->ftilt_threshold && horz; }
	bool bthrow() const { return stick.x * direction <= -plco->ftilt_threshold && horz; }
	bool uthrow() const { return stick.y             >=  plco->utilt_threshold && vert; }
	bool dthrow() const { return stick.y             <=  plco->dtilt_threshold && vert; }
};

static int get_throw_state(const Player *player, bool vanilla = false, bool check_cstick = true)
{
	// fthrow > bthrow > c-fthrow > c-bthrow > uthrow > c-uthrow > dthrow > c-dthrow

	const auto curr   = checker { player->input.stick,       player->direction, vanilla };
	const auto prev   = checker { player->input.last_stick,  player->direction, vanilla };
	const auto curr_c = checker { player->input.cstick,      player->direction, vanilla };
	const auto prev_c = checker { player->input.last_cstick, player->direction, vanilla };

	if (curr.fthrow()   && !prev.fthrow())                   return AS_ThrowF;
	if (curr.bthrow()   && !prev.bthrow())                   return AS_ThrowB;
	if (curr_c.fthrow() && !prev_c.fthrow() && check_cstick) return AS_ThrowF;
	if (curr_c.bthrow() && !prev_c.bthrow() && check_cstick) return AS_ThrowB;
	if (curr.uthrow()   && !prev.uthrow())                   return AS_ThrowHi;
	if (curr_c.uthrow() && !prev_c.uthrow() && check_cstick) return AS_ThrowHi;
	if (curr.dthrow()   && !prev.dthrow())                   return AS_ThrowLw;
	if (curr_c.dthrow() &&  prev_c.dthrow() && check_cstick) return AS_ThrowLw; // buffer bug

	// Fall back to vanilla logic
	// Don't check cstick fallback on the c-down delay frame
	if (!vanilla)
		return get_throw_state(player, true, !curr_c.dthrow());

	return AS_None;
}

extern "C" bool orig_Interrupt_Throw(HSD_GObj *gobj);
extern "C" bool hook_Interrupt_Throw(HSD_GObj *gobj)
{
	if (get_cfix() < cfix::b)
		return orig_Interrupt_Throw(gobj);

	const auto *player = gobj->get<Player>();

	// Disable for ICs to avoid breaking desync tech
	if (player->character_id == CID_Popo || player->character_id == CID_Nana)
		return orig_Interrupt_Throw(gobj);

	// Check for throws with 50d line
	if (const auto state = get_throw_state(player); state != AS_None) {
		Player_DoThrow(gobj, state);
		return true;
	}

	return false;
}

static int get_cargo_throw_state(const Player *player)
{
	const auto check = checker { player->input.stick, player->direction, false };

	if (check.fthrow()) return AS_DK_ThrowFF;
	if (check.bthrow()) return AS_DK_ThrowFB;
	if (check.uthrow()) return AS_DK_ThrowFHi;
	if (check.dthrow()) return AS_DK_ThrowFLw;

	return AS_None;
}

extern "C" bool orig_Interrupt_DK_ThrowFDecide(HSD_GObj *gobj);
extern "C" bool hook_Interrupt_DK_ThrowFDecide(HSD_GObj *gobj)
{
	if (get_cfix() < cfix::b)
		return orig_Interrupt_DK_ThrowFDecide(gobj);

	const auto *player = gobj->get<Player>();

	// Cargo throw requires A/B press
	if (!(player->input.instant_buttons & (Button_A | Button_B)))
		return false;

	// Check for throws with 50d line
	if (const auto state = get_cargo_throw_state(player); state != AS_None) {
		AS_DK_ThrowFDecide(gobj, state);
		return true;
	}

	return false;
}
#include "hsd/gobj.h"
#include "melee/constants.h"
#include "melee/object.h"
#include "melee/player.h"
#include "rules/values.h"
#include <gctypes.h>

static bool check_sdi_magnitude(const vec2 &stick)
{
	// Check .7 magnitude
	return stick.length_sqr() >= plco->sdi_stick_threshold * plco->sdi_stick_threshold;
}

extern "C" void orig_Player_EnterDamageState(HSD_GObj *gobj, u32 state, f32 direction);
extern "C" void hook_Player_EnterDamageState(HSD_GObj *gobj, u32 state, f32 direction)
{
	orig_Player_EnterDamageState(gobj, state, direction);

	if (get_ucf_type() == ucf_type::ucf)
		return;

	auto *player = gobj->get<Player>();

	// Only allow f2 SDI if below .7 magnitude
	player->as_data.Damage.allow_f2_sdi = !check_sdi_magnitude(player->input.stick);
	player->as_data.Damage.hitlag_frame = 1;
	player->as_data.Damage.allow_sdi_extension = false;
}

static bool check_sdi_window(const Player *player)
{
	if (player->as_data.Damage.hitlag_frame == 2 && player->as_data.Damage.allow_f2_sdi) {
		// Allow a SDI on the 2nd frame of hitlag after leaving the deadzone on f1
		const auto x_frames = player->input.true_stick_x_hold_time;
		const auto y_frames = player->input.true_stick_y_hold_time;

		if (x_frames >= 2 && y_frames >= 2)
			return false;
	} else {
		// Check 4f smash input
		const auto x_frames = player->input.stick_x_hold_time;
		const auto y_frames = player->input.stick_y_hold_time;

		if (x_frames >= plco->sdi_stick_frames && y_frames >= plco->sdi_stick_frames)
			return false;
	}

	return true;
}

static bool is_valid_extension(const vec2 &stick, const vec2 &last_stick)
{
	// Magnitude must increase
	if (stick.length_sqr() <= last_stick.length_sqr())
		return false;

	// Don't allow going from quadrant to cardinal
	if (stick.x == 0.f && last_stick.x != 0.f)
		return false;

	if (stick.y == 0.f && last_stick.y != 0.f)
		return false;

	return true;
}

static bool check_sdi(const Player *player, bool *is_extension)
{
	const auto &input = player->input;

	// Check .7 magnitude
	if (!check_sdi_magnitude(input.stick))
		return false;

	if (check_sdi_window(player)) {
		*is_extension = false;
		return true;
	}

	if (!player->as_data.Damage.allow_sdi_extension)
		return false;

	// Check for a valid extension input
	if (!is_valid_extension(input.stick, input.last_stick)) {
		*is_extension = true;
		return true;
	}

	return false;
}

// Get the position the player would be on the next frame of hitlag with no SDI
static vec3 get_next_position(const Player *player)
{
	auto position = player->position;

	// Add floor speed
	vec3 speed;
	if (!player->airborne && Collision_GetLineSpeed(player->phys.floor.line, position, &speed))
		position += speed;

	// Add wind push (whispy)
	vec3 push;
	Player_GetWindPush(player->gobj, &push);

	return position + push;
}

extern "C" void orig_Player_SDICallback(HSD_GObj *gobj);
extern "C" void hook_Player_SDICallback(HSD_GObj *gobj)
{
	if (get_ucf_type() == ucf_type::ucf)
		return orig_Player_SDICallback(gobj);

	auto *player = gobj->get<Player>();
	auto *as_data = &player->as_data.Damage;

	if (!player->in_hitlag)
		return;

	as_data->hitlag_frame++;

	bool is_extension;

	if (!check_sdi(player, &is_extension)) {
		as_data->allow_sdi_extension = false;
		return;
	} else if (as_data->allow_sdi_extension) {
		as_data->allow_sdi_extension = false;
	} else if (!check_sdi_magnitude(player->input.last_stick)) {
		// Allow SDI to be extended to full magnitude the frame after entering SDI range
		as_data->allow_sdi_extension = true;
		as_data->sdi_extension_start = get_next_position(player);
	}

	// Don't trigger again until a new cardinal is added
	player->input.stick_x_hold_time = 0xFE;
	player->input.stick_y_hold_time = 0xFE;

	if (is_extension) {
		// Remove old SDI distance from stats
		const auto old_delta = player->input.last_stick * plco->sdi_distance;
		auto *stats = PlayerBlock_GetStats(player->slot);
		stats->total_sdi_distance_x -= std::abs(old_delta.x);
		stats->total_sdi_distance_y -= std::abs(old_delta.y);

		// Use position from before initial SDI
		player->position = as_data->sdi_extension_start;
	}

	const auto delta = player->input.stick * plco->sdi_distance;
	player->position.x += delta.x;
	player->position.y += delta.y;
	PlayerBlock_AddTotalSDIDistance(player->slot, delta.x, delta.y);
}
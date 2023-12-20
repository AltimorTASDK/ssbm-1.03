#include "hsd/gobj.h"
#include "melee/constants.h"
#include "melee/object.h"
#include "melee/player.h"
#include "rules/values.h"
#include "util/melee/pad.h"
#include <gctypes.h>

// Custom AS data
struct sdi_data {
	u32 hitlag_frame;
	// Position before initial SDI
	vec3 sdi_extension_start;
	// Allow SDI on frame 2 of hitlag if below SDI magnitude on f1, even if
	// outside the deadzone on f1
	bool allow_f2_sdi;
	// Allow extending SDI magnitude on the next frame
	bool allow_sdi_extension;
};

static bool check_sdi_magnitude(const vec2 &stick)
{
	// Check .7 magnitude
	return stick.length_sqr() >= plco->sdi_stick_threshold * plco->sdi_stick_threshold;
}

static bool check_shield_sdi_x(const vec2 &stick)
{
	// Check .7 X
	return std::fabs(stick.x) >= plco->sdi_stick_threshold;
}

extern "C" void orig_Player_EnterDamageState(HSD_GObj *gobj, u32 state, f32 direction);
extern "C" void hook_Player_EnterDamageState(HSD_GObj *gobj, u32 state, f32 direction)
{
	orig_Player_EnterDamageState(gobj, state, direction);

	if (get_cfix() < cfix::b)
		return;

	auto *player = gobj->get<Player>();

	// Only allow f2 SDI if below .7 magnitude
	auto *as_data = player->custom_as_data<sdi_data>();
	as_data->allow_f2_sdi = !check_sdi_magnitude(player->input.stick);
	as_data->hitlag_frame = 1;
	as_data->allow_sdi_extension = false;
}

extern "C" void orig_Player_ShieldHitCallback(HSD_GObj *gobj);
extern "C" void hook_Player_ShieldHitCallback(HSD_GObj *gobj)
{
	orig_Player_ShieldHitCallback(gobj);

	if (get_cfix() < cfix::b)
		return;

	auto *player = gobj->get<Player>();

	// Only allow f2 SDI if below .7 X
	auto *as_data = player->custom_as_data<sdi_data>();
	as_data->allow_f2_sdi = !check_shield_sdi_x(player->input.stick);
	as_data->hitlag_frame = 1;
	as_data->allow_sdi_extension = false;
}

static bool check_sdi_window(const Player *player)
{
	const auto *as_data = player->custom_as_data<sdi_data>();

	if (as_data->hitlag_frame == 2 && as_data->allow_f2_sdi) {
		// Allow a SDI on the 2nd frame of hitlag after leaving the deadzone on f1
		const auto x_frames = player->input.true_stick_x_hold_time;
		const auto y_frames = player->input.true_stick_y_hold_time;
		return x_frames < 2 || y_frames < 2;
	} else {
		// Check 4f smash input
		const auto x_frames = player->input.stick_x_hold_time;
		const auto y_frames = player->input.stick_y_hold_time;
		return x_frames < plco->sdi_stick_frames || y_frames < plco->sdi_stick_frames;
	}
}

static bool check_shield_sdi_window(const Player *player)
{
	const auto *as_data = player->custom_as_data<sdi_data>();

	if (as_data->hitlag_frame == 2 && as_data->allow_f2_sdi) {
		// Allow a SDI on the 2nd frame of hitlag after leaving the deadzone on f1
		const auto x_frames = player->input.true_stick_x_hold_time;
		return x_frames < 2;
	} else {
		// Check 4f smash input
		const auto x_frames = player->input.stick_x_hold_time;
		return x_frames < plco->sdi_stick_frames;
	}
}

static bool is_valid_sdi_extension(const vec2 &stick, const vec2 &last_stick)
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

static bool is_valid_shield_sdi_extension(const vec2 &stick, const vec2 &last_stick)
{
	// X must increase
	if (last_stick.x > 0)
		return stick.x > last_stick.x;
	else
		return stick.x < last_stick.x;
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

	if (get_cfix() < cfix::b || !player->custom_as_data<sdi_data>()->allow_sdi_extension)
		return false;

	// Check for a valid extension input
	if (is_valid_sdi_extension(input.stick, input.last_stick)) {
		*is_extension = true;
		return true;
	}

	return false;
}

static bool check_shield_sdi(const Player *player, bool *is_extension)
{
	const auto &input = player->input;

	// Check .7 X
	if (!check_shield_sdi_x(input.stick))
		return false;

	if (check_shield_sdi_window(player)) {
		*is_extension = false;
		return true;
	}

	if (get_cfix() < cfix::b || !player->custom_as_data<sdi_data>()->allow_sdi_extension)
		return false;

	// Check for a valid extension input
	if (is_valid_shield_sdi_extension(input.stick, input.last_stick)) {
		*is_extension = true;
		return true;
	}

	return false;
}

// Simulate player collision for this frame
static vec3 trace_collision(const Player *player, const vec3 &trace_end)
{
	auto phys = player->phys;

	// Check if ECB should reset on this frame
	if (player->ecb_timer == 1)
		phys.ecb_flags &= ~ECBFlag_FreezeBottom;

	phys.start_position = phys.position;
	phys.position = trace_end;

	if (player->airborne)
		Physics_Collision_Air_StayAirborne(&phys);
	else
		Physics_Collision_Grounded(&phys);

	return phys.position;
}

// Get the position the player would be on the next frame of hitlag with no SDI
static vec3 get_next_position(const Player *player)
{
	auto position = player->position;

	// Add floor speed
	vec3 speed;
	if (!player->airborne && Collision_GetLineSpeed(player->phys.floor.line, position, &speed))
		position += speed;

	return trace_collision(player, position);
}

extern "C" void orig_Player_SDICallback(HSD_GObj *gobj);
extern "C" void hook_Player_SDICallback(HSD_GObj *gobj)
{
	if (get_cfix() < cfix::a)
		return orig_Player_SDICallback(gobj);

	auto *player = gobj->get<Player>();
	auto *as_data = player->custom_as_data<sdi_data>();
	const auto &input = player->input;

	if (!player->in_hitlag)
		return;

	as_data->hitlag_frame++;

	bool is_extension;

	if (!check_sdi(player, &is_extension)) {
		as_data->allow_sdi_extension = false;
		return;
	} else if (as_data->allow_sdi_extension) {
		as_data->allow_sdi_extension = false;
	} else if (!check_sdi_magnitude(input.last_stick) && !is_rim_coord(input.stick)) {
		// Allow SDI to be extended to full magnitude the frame after entering SDI range
		as_data->allow_sdi_extension = true;
		as_data->sdi_extension_start = get_next_position(player);
	}

	// Don't trigger again until a new cardinal is added
	player->input.stick_x_hold_time = 0xFE;
	player->input.stick_y_hold_time = 0xFE;

	if (is_extension) {
		// Remove old SDI distance from stats
		const auto old_delta = input.last_stick * plco->sdi_distance;
		auto *stats = PlayerBlock_GetStats(player->slot);
		stats->total_sdi_distance_x -= std::abs(old_delta.x);
		stats->total_sdi_distance_y -= std::abs(old_delta.y);

		// Use position from before initial SDI
		player->position = as_data->sdi_extension_start;
		player->phys.position = player->position;
	}

	const auto delta = input.stick * plco->sdi_distance;
	player->position.x += delta.x;
	player->position.y += delta.y;
	PlayerBlock_AddTotalSDIDistance(player->slot, delta.x, delta.y);
}

extern "C" void orig_Player_ShieldSDICallback(HSD_GObj *gobj);
extern "C" void hook_Player_ShieldSDICallback(HSD_GObj *gobj)
{
	if (get_cfix() < cfix::a)
		return orig_Player_ShieldSDICallback(gobj);

	auto *player = gobj->get<Player>();
	auto *as_data = player->custom_as_data<sdi_data>();
	const auto &input = player->input;

	if (!player->in_hitlag || player->airborne)
		return;

	as_data->hitlag_frame++;

	bool is_extension;

	if (!check_shield_sdi(player, &is_extension)) {
		as_data->allow_sdi_extension = false;
		return;
	} else if (as_data->allow_sdi_extension) {
		as_data->allow_sdi_extension = false;
	} else if (!check_shield_sdi_x(input.last_stick) && std::fabs(input.stick.x) != 1.f) {
		// Allow SDI to be extended to full distance the frame after entering SDI range
		as_data->allow_sdi_extension = true;
		as_data->sdi_extension_start = get_next_position(player);
	}

	// Don't trigger again until X deadzone is crossed
	player->input.stick_x_hold_time = 0xFE;

	if (is_extension) {
		// Use position from before initial SDI
		player->position = as_data->sdi_extension_start;
		player->phys.position = player->position;
	}

	// Move player perpendicularly to floor normal
	const auto delta = input.stick.x * plco->sdi_distance * plco->shield_sdi_mult;
	player->position.x += player->phys.floor.normal.y * delta;
	player->position.y -= player->phys.floor.normal.x * delta;
}
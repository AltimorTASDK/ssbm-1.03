#include "melee/player.h"
#include "melee/characters/samus.h"

static struct {
	float x_vel_history[2];
	int frames_since_swd = 10000;
} swd_data[6];

static bool is_grounded_morph_ball(Player *player)
{
	return player->character_id == CID_Samus && player->action_state == AS_Samus_SpecialLw;
}

static void check_swd_extension(Player *player)
{
	if (!is_grounded_morph_ball(player))
		return;

	// must be frame after swd
	if (swd_data[player->slot].frames_since_swd != 1)
		return;
		
	const auto stick_delta = player->input.stick.x - player->input.last_stick.x;

	// stick value must have increased in same direction
	if (stick_delta * player->input.last_stick.x <= 0.f)
		return;
		
	const auto &samus_stats = player->extra_stats.special_stats->samus;
	const auto morph_accel = player->char_stats.walk_accel_extra * samus_stats.morph_accel_mult;
	const auto vel_delta = stick_delta * morph_accel;
	
	// update velocity and add lost movement from previous frame to target position
	player->ground_vel += vel_delta;
	player->position.x += vel_delta;
}

extern "C" void orig_PlayerThink_Physics(HSD_GObj *gobj);
extern "C" void hook_PlayerThink_Physics(HSD_GObj *gobj)
{
	auto *player = gobj->get<Player>();
	
	swd_data[player->slot].x_vel_history[1] = swd_data[player->slot].x_vel_history[0];
	swd_data[player->slot].x_vel_history[0] = player->move_vel.x;
	swd_data[player->slot].frames_since_swd++;
	
	check_swd_extension(player);

	orig_PlayerThink_Physics(gobj);
}

extern "C" void orig_Player_SpecialAccel(Player *player, float deadzone, float accel, float max_speed);
extern "C" void hook_Player_SpecialAccel(Player *player, float deadzone, float accel, float max_speed)
{
	if (!is_grounded_morph_ball(player)) {
		orig_Player_SpecialAccel(player, deadzone, accel, max_speed);
		return;
	}
	
	const auto vel = player->ground_vel;
	auto speed_cap = player->input.stick.x * max_speed;
	auto vel_delta = player->input.stick.x * accel;
	
	if (speed_cap == 0.f) {
		// instant stop
		vel_delta = -vel;
	} else if (vel * vel_delta >= 0.f) {
		// this is the starting velocity from last frame, which is the final velocity from 2 frames ago
		const auto old_vel = swd_data[player->slot].x_vel_history[1];
	
		if (vel == 0.f && old_vel * vel_delta < 0.f) {
			// successfully SWD as if starting from our velocity before the deadzone poll
			vel_delta += old_vel;
			swd_data[player->slot].frames_since_swd = 0;
		} else if (std::abs(vel + vel_delta) > std::abs(speed_cap)) {
			vel_delta = speed_cap - vel;
		}
	} else {
		swd_data[player->slot].frames_since_swd = 0;
	}

	player->ground_accel = vel_delta;
}
#include "melee/player.h"

static struct {
	float drift_vel_difference;
	float intermediate_stick_x;
} wj_data[6];

extern "C" void Physics_AirAcceleration(Player *player, float x_vel, float accel, float max_speed, float friction);

static float simulate_air_physics(Player *player, float x_vel, float stick_x)
{
	const auto &stats = player->char_stats;
	const auto base_accel = stick_x > 0.f ? stats.air_accel_base : -stats.air_accel_base;
	const auto accel = stick_x * stats.air_accel_extra + base_accel;
	const auto max_speed = stick_x * stats.air_speed;

	const auto vel_delta_backup = player->move_vel_delta.x;
	Physics_AirAcceleration(player, x_vel, accel, max_speed, stats.air_friction);
	const auto vel_delta = player->move_vel_delta.x;
	player->move_vel_delta.x = vel_delta_backup;

	return vel_delta;
}

extern "C" void orig_Player_AirMovement(Player *player, float x_vel);
extern "C" void hook_Player_AirMovement(Player *player, float x_vel)
{
	// Store difference in what velocity would've been without a potential intermediate stick pos
	const auto wj_fix_vel_delta = simulate_air_physics(player, x_vel, player->input.last_stick.x);

	orig_Player_AirMovement(player, x_vel);

	wj_data[player->slot].intermediate_stick_x = player->input.stick.x;
	wj_data[player->slot].drift_vel_difference = wj_fix_vel_delta - player->move_vel_delta.x;
}

extern "C" void orig_PlayerThink_Collision(HSD_GObj *gobj);
extern "C" void hook_PlayerThink_Collision(HSD_GObj *gobj)
{
	//const auto *player = gobj->get<Player>();
	//auto phys = player->phys;

	//// Check if ECB should reset on this frame
	//if (player->ecb_timer == 1)
	//	phys.ecb_flags &= ~ECBFlag_FreezeBottom;

	//phys.start_position = phys.position;
	//phys.position = player->position + vec3(wj_data[player->slot].drift_vel_difference, 0.f, 0.f);

	orig_PlayerThink_Collision(gobj);
}
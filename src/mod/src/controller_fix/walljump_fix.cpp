#include "melee/player.h"

/*static struct {
	float drift_vel;
} wj_data[6];

extern "C" void orig_Player_AirMovement(Player *player, float x_vel);
extern "C" void hook_Player_AirMovement(Player *player, float x_vel)
{
	orig_Player_AirMovement(player, x_vel);
	wj_data[player->slot].drift_vel = player->move_vel_delta.x;
}*/
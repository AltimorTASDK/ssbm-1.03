#pragma once

#include <gctypes.h>

struct PlayerConstants {
	char pad000[0x3C];
	f32 x_smash_threshold;
	s32 x_smash_frames;
	char pad044[0x90 - 0x44];
	float squat_threshold;
	float max_squatwait_threshold;
	char pad098[0x314 - 0x98];
	f32 spot_dodge_stick_threshold;
	u32 spot_dodge_stick_frames;
	f32 roll_stick_threshold;
	u32 roll_stick_frames;
};

extern "C" PlayerConstants *plco;
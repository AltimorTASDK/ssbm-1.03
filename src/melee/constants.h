#pragma once

#include <gctypes.h>

struct PlayerConstants {
	char pad000[0x3C];
	f32 x_smash_threshold;
	s32 x_smash_frames;
	char pad044[0x90 - 0x44];
	f32 squat_threshold;
	f32 max_squatwait_threshold;
	char pad098[0x210 - 0x98];
	f32 tumble_wiggle_threshold;
	s32 tumble_wiggle_frames;
	char pad218[0x314 - 0x218];
	f32 spot_dodge_stick_threshold;
	s32 spot_dodge_stick_frames;
	f32 roll_stick_threshold;
	s32 roll_stick_frames;
};

extern "C" PlayerConstants *plco;
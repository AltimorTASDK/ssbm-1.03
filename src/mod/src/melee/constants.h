#pragma once

#include "util/vector.h"
#include <gctypes.h>

struct PlayerConstants {
	vec2 deadzone;
	char pad008[0x20 - 0x08];
	f32 angle_50d;
	f32 x_tilt_threshold;
	char pad028[0x3C - 0x28];
	f32 x_smash_threshold;
	s32 x_smash_frames;
	char pad044[0x70 - 0x44];
	f32 y_smash_threshold;
	s32 y_smash_frames;
	char pad078[0x90 - 0x78];
	f32 squat_threshold;
	f32 max_squatwait_threshold;
	f32 ftilt_threshold;
	f32 uftilt_angle;
	f32 uftilt_mid_angle;
	f32 dftilt_mid_angle;
	f32 dftilt_angle;
	f32 utilt_threshold;
	f32 dtilt_threshold;
	char pad0B4[0xCC - 0xB4];
	f32 usmash_threshold;
	f32 usmash_frames;
	f32 dsmash_threshold;
	f32 dsmash_frames;
	char pad0DC[0x210 - 0xDC];
	f32 tumble_wiggle_threshold;
	s32 tumble_wiggle_frames;
	char pad218[0x314 - 0x218];
	f32 spot_dodge_stick_threshold;
	s32 spot_dodge_stick_frames;
	f32 roll_stick_threshold;
	s32 roll_stick_frames;
	char pad324[0x4B0 - 0x324];
	f32 sdi_stick_threshold;
	s32 sdi_stick_frames;
	f32 sdi_distance;
	char pad4BC[0x5F0 - 0x4BC];
	s32 idle_timer;
};

extern "C" PlayerConstants *plco;
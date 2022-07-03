#pragma once

#include "util/vector.h"
#include <gctypes.h>

struct PlayerConstants {
	vec2 deadzone;
	vec2 stick_hold_threshold;
	f32 lr_deadzone;
	f32 z_analog_value;
	f32 lr_hold_threshold;
	s32 tech_lockout;
	f32 angle_50d;
	f32 walk_threshold;
	char pad028[0x3C - 0x28];
	f32 x_smash_threshold;
	s32 x_smash_frames;
	char pad044[0x58 - 0x44];
	f32 run_threshold;
	char pad05C[0x70 - 0x5C];
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
	f32 aerial_threshold_x;
	f32 aerial_threshold_y;
	char pad0E4[0x210 - 0xE4];
	f32 tumble_wiggle_threshold;
	s32 tumble_wiggle_frames;
	f32 x_special_threshold;
	f32 y_special_threshold;
	char pad220[0x314 - 0x220];
	f32 spot_dodge_stick_threshold;
	s32 spot_dodge_stick_frames;
	f32 roll_stick_threshold;
	s32 roll_stick_frames;
	char pad324[0x480 - 0x324];
	f32 pass_ledge_threshold;
	char pad484[0x494 - 0x484];
	f32 ledge_deadzone;
	s32 ledge_regrab_timer;
	s32 ledge_iframes;
	char pad4A0[0x4B0 - 0x4A0];
	f32 sdi_stick_threshold;
	s32 sdi_stick_frames;
	f32 sdi_distance;
	f32 asdi_distance;
	f32 shield_sdi_mult;
	char pad4C4[0x5F0 - 0x4C4];
	s32 idle_timer;
	char pad5F4[0x76C - 0x5F4];
	f32 walljump_stick_threshold;
};

extern "C" PlayerConstants *plco;
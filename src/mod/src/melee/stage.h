#pragma once

#include "util/vector.h"
#include <gctypes.h>

enum StageID {
	Stage_FoD = 2,
	Stage_PS  = 3,
	Stage_YS  = 8,
	Stage_DL  = 28,
	Stage_BF  = 31,
	Stage_FD  = 32
};

union StageParams {
	struct {
		f32 left_plat_height;
		f32 pad004;
		f32 right_plat_height;
	} fod;
	struct {
		char pad000[8];
		s32 whispy_push_timer_min;
		s32 whispy_push_timer_max;
		f32 whispy_push_strength;
		f32 whispy_right_push_bounds_right;
		f32 whispy_right_push_bounds_left;
		f32 whispy_left_push_bounds_left;
		f32 whispy_left_push_bounds_right;
		f32 whispy_push_bounds_bottom;
		f32 whispy_push_bounds_top;
		s32 whispy_blink_timer_min;
		s32 whispy_blink_timer_max;
	} dl;
};

struct StageObject {
	char pad000[0xCC];
	// Whispy stuff, probably in a union
	s32 blink_timer;
	s32 push_timer;
};

extern "C" {
	
extern u16 StageIndexToID[29];

StageParams *Stage_GetParameters();
void Stage_GetSpawnPoint(u32 slot, vec3 *spawn);
u32 Stage_GetID();

}
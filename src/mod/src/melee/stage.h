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

struct StageIDPair {
	u32 stkind;
	u32 grkind;
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

struct StageData {
	char pad000[0x8C];
	struct {
		u8 flags_80 : 1;
		u8 flags_40 : 1;
		u8 flags_20 : 1;
		u8 flags_10 : 1;
		u8 multiple_respawn_points : 1;
	};
	char pad08D[0x6A8 - 0x8D];
	void *itemdata;
	void *coll_data;
	void *ground_params;
	void *ald_yaku_all;
	void *map_ptcl;
	void *map_texg;
	StageParams *parameters;
	void *map_plit;
};

extern "C" {

extern u16 StageIndexToID[29];
extern StageData Stage;

StageParams *Stage_GetParameters();
void Stage_GetSpawnPoint(u32 slot, vec3 *spawn);
u32 Stage_GetID();

f32 Stage_GetCameraLimitLeft();
f32 Stage_GetCameraLimitRight();
f32 Stage_GetCameraLimitTop();
f32 Stage_GetCameraLimitBottom();

} // extern "C"
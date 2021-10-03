#pragma once

#include "util/vector.h"
#include <gctypes.h>

struct PADStatus {
	u16 buttons;
	vec2c stick;
	vec2c cstick;
	s8 analog_l;
	s8 analog_r;
	s8 analog_a;
	s8 analog_b;
	s8 err;
};

struct HSD_PadData {
	PADStatus stat[4];
};

struct PadLibData {
	u8 qnum;
	u8 qread;
	u8 qwrite;
	u8 qcount;
	u8 qtype;
	HSD_PadData *queue;
	s32 repeat_start;
	s32 repeat_interval;
	u8 adc_type;
	s8 adc_th;
	f32 adc_angle;
	u8 clamp_stickType;
	u8 clamp_stickShift;
	s8 clamp_stickMax;
	s8 clamp_stickMin;
	u8 clamp_analogLRShift;
	u8 clamp_analogLRMax;
	u8 clamp_analogLRMin;
	u8 clamp_analogABShift;
	u8 clamp_analogABMax;
	u8 clamp_analogABMin;
	s8 scale_stick;
	u8 scale_analogLR;
	u8 scale_analogAB;
	u8 cross_dir;
	u8 reset_switch_status;
	u8 reset_switch;
	void *rumble_info;
};

extern "C" PadLibData HSD_PadLibData;
#pragma once

#include "dolphin/pad.h"
#include "util/vector.h"
#include <bit>
#include <gctypes.h>

constexpr auto PAD_QNUM = 5;

enum HSD_Button {
	Button_AnalogLR = 0x80000000
};

enum HSD_ButtonBit {
	ButtonBit_AnalogLR = std::countr_zero((u32)Button_AnalogLR)
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

struct HSD_PadStatus {
	u32 buttons;
	u32 last_buttons;
	u32 instant_buttons;
	u32 repeated_buttons;
	u32 released_buttons;
	s32 repeat_count;
	vec2c raw_stick;
	vec2c raw_cstick;
	u8 raw_analog_l;
	u8 raw_analog_r;
	u8 raw_analog_a;
	u8 raw_analog_b;
	vec2 stick;
	vec2 cstick;
	f32 analog_l;
	f32 analog_r;
	f32 analog_a;
	f32 analog_b;
	u8 cross_dir;
	s8 err;
};

extern "C" {

extern HSD_PadStatus HSD_PadMasterStatus[4];
extern HSD_PadStatus HSD_PadCopyStatus[4];
extern PadLibData HSD_PadLibData;

void HSD_PadRumble(u32 port, s32 id1, s32 id2, s32 duration);

void HSD_PadClamp(s8 *x, s8 *y, bool shift, s8 min, s8 max);

} // extern "C"
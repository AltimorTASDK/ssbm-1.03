#pragma once

#include "util/vector.h"
#include <gctypes.h>

constexpr auto PAD_QNUM = 5;

enum Button {
	Button_DPadLeft = 1,
	Button_DPadRight = 2,
	Button_DPadDown = 4,
	Button_DPadUp = 8,
	Button_Z = 0x10,
	Button_R = 0x20,
	Button_L = 0x40,
	Button_A = 0x100,
	Button_B = 0x200,
	Button_X = 0x400,
	Button_Y = 0x800,
	Button_Start = 0x1000,
	Button_AnalogLR = 0x80000000
};

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
	float analog_l;
	float analog_r;
	float analog_a;
	float analog_b;
	u8 cross_dir;
	s8 err;
};

extern "C" {

extern HSD_PadStatus HSD_PadMasterStatus[4];
extern PadLibData HSD_PadLibData;

void HSD_PadRumble(u32 port, s32 id1, s32 id2, s32 duration);

}
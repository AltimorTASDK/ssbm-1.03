#pragma once

#include "util/vector.h"
#include <bit>
#include <gctypes.h>

enum Button {
	Button_DPadLeft  = 0x0001,
	Button_DPadRight = 0x0002,
	Button_DPadDown  = 0x0004,
	Button_DPadUp    = 0x0008,
	Button_Z         = 0x0010,
	Button_R         = 0x0020,
	Button_L         = 0x0040,
	Button_UseOrigin = 0x0080,
	Button_A         = 0x0100,
	Button_B         = 0x0200,
	Button_X         = 0x0400,
	Button_Y         = 0x0800,
	Button_Start     = 0x1000,
	Button_GetOrigin = 0x2000
};

enum ButtonBit {
	ButtonBit_DPadLeft  = std::countr_zero((u32)Button_DPadLeft),
	ButtonBit_DPadRight = std::countr_zero((u32)Button_DPadRight),
	ButtonBit_DPadDown  = std::countr_zero((u32)Button_DPadDown),
	ButtonBit_DPadUp    = std::countr_zero((u32)Button_DPadUp),
	ButtonBit_Z         = std::countr_zero((u32)Button_Z),
	ButtonBit_R         = std::countr_zero((u32)Button_R),
	ButtonBit_L         = std::countr_zero((u32)Button_L),
	ButtonBit_UseOrigin = std::countr_zero((u32)Button_UseOrigin),
	ButtonBit_A         = std::countr_zero((u32)Button_A),
	ButtonBit_B         = std::countr_zero((u32)Button_B),
	ButtonBit_X         = std::countr_zero((u32)Button_X),
	ButtonBit_Y         = std::countr_zero((u32)Button_Y),
	ButtonBit_Start     = std::countr_zero((u32)Button_Start),
	ButtonBit_GetOrigin = std::countr_zero((u32)Button_GetOrigin)
};

struct PADStatus {
	u16 buttons;
	vec2c stick;
	vec2c cstick;
	u8 analog_l;
	u8 analog_r;
	u8 analog_a;
	u8 analog_b;
	u8 err;
};
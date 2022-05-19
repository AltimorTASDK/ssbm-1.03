#pragma once

#include "util/vector.h"
#include <gctypes.h>

enum Button {
	Button_DPadLeft  = 0x0001,
	Button_DPadRight = 0x0002,
	Button_DPadDown  = 0x0004,
	Button_DPadUp    = 0x0008,
	Button_Z         = 0x0010,
	Button_R         = 0x0020,
	Button_L         = 0x0040,
	Button_A         = 0x0100,
	Button_B         = 0x0200,
	Button_X         = 0x0400,
	Button_Y         = 0x0800,
	Button_Start     = 0x1000
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
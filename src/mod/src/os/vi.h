#pragma once

#include <gctypes.h>

union display_interrupt {
	struct {
		u32 status : 1;
		u32 : 2;
		u32 enable : 1;
		u32 : 2;
		u32 position_v : 10;
		u32 : 6;
		u32 position_h : 10;
	} bits;
	u32 raw;
};

struct vi_regdef {
	char pad000[0x2C];
	u16 scan_v;
	u16 scan_h;
	display_interrupt pre_retrace;
	display_interrupt post_retrace;
	display_interrupt interrupt2;
	display_interrupt interrupt3;
	char pad040[0x48 - 0x40];
	union {
		struct {
			u32 : 1;
		};
		u16 raw;
	} picture_config;
};

inline volatile auto *vi_regs = (vi_regdef*)0xCC002000;

extern "C" {

u32 VIGetCurrentLine();
u32 VIGetRetraceCount();
void VIWaitForRetrace();
void VISetNextFrameBuffer(void *buffer);

} // extern "C"
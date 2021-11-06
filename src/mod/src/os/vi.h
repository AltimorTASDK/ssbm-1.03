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
	u32 word;
};

struct vi_regdef {
	char pad000[0x2C];
	u16 scan_h;
	u16 scan_v;
	display_interrupt display_int[4];
};
	
inline volatile auto *vi_regs = (vi_regdef*)0xCC002000;

extern "C" {

u32 VIGetCurrentLine();
u32 VIGetRetraceCount();
void VIWaitForRetrace();

}
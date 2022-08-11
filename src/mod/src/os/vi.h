#pragma once

#include <gctypes.h>
#include <ogc/gx_struct.h>

constexpr auto VI_DISPLAY_PIX_SZ = 2;

enum VITVFormat {
	VI_NTSC      = 0,
	VI_PAL       = 1,
	VI_MPAL      = 2,
	VI_DEBUG     = 3,
	VI_DEBUG_PAL = 4,
	VI_EURGB60   = 5
};

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
	struct {
		u16 : 2;
		u16 acv : 10;
		u16 equ : 4;
	} vtr;
	struct {
		u16 : 6;
		u16 fmt : 2;
		u16 le1 : 2;
		u16 le0 : 2;
		u16 dlr : 1;
		u16 nin : 1;
		u16 rst : 1;
		u16 enb : 1;
	} dcr;
	union {
		struct {
			u32 : 1;
			u32 hcs : 7;
			u32 : 1;
			u32 hce : 7;
			u32 : 7;
			u32 hlw : 9;
		};
		struct {
			u16 hi;
			u16 lo;
		};
	} htr0;
	union {
		struct {
			u32 : 5;
			u32 hbs : 10;
			u32 hbe : 10;
			u32 hsy : 7;
		};
		struct {
			u16 hi;
			u16 lo;
		};
	} htr1;
	char pad00C[0x2C - 0x0C];
	u16 scan_v;
	u16 scan_h;
	display_interrupt pre_retrace;
	display_interrupt post_retrace;
	display_interrupt interrupt2;
	display_interrupt interrupt3;
	char pad040[0x6C - 0x40];
	union {
		struct {
			u16 : 15;
			u16 s : 1;
		};
		u16 raw;
	} viclk;
	char pad06E[0x78 - 0x6E];
};

inline volatile auto *vi_regs = (vi_regdef*)0xCC002000;

extern "C" {

u32 VIGetCurrentLine();
u32 VIGetRetraceCount();
void VIWaitForRetrace();

void VISetNextFrameBuffer(void *buffer);

void VIConfigure(const GXRModeObj *rmode);
void VIFlush();

} // extern "C"
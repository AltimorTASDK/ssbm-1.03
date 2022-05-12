#pragma once

#include <gctypes.h>
#include <ogc/si.h>
#include "util/vector.h"

union SIPoll {
	u32 raw;
	struct {
		u32 : 6;
		u32 x : 10;
		u32 y : 8;
		u32 enable : 4;
		u32 vblank_copy : 4;
	};
};

struct SIControl {
	char pad000[0x04];
	SIPoll poll;
	char pad008[0x14 - 0x08];
};

struct SIStatus {
	u8 wr : 1;
	u8 : 1;
	u8 rdst : 1;
	u8 wrst : 1;
	u8 norep : 1;
	u8 coll : 1;
	u8 ovrun : 1;
	u8 unrun : 1;
};

struct SIChannel {
	u32 out;
	union {
		struct {
			u32 hi;
			u32 lo;
		};
		struct {
			u16 errstat : 1;
			u16 errlatch : 1;
			u16 buttons : 14;
			s8 stick_x;
			s8 stick_y;
			s8 cstick_x;
			s8 cstick_y;
			u8 analog_l;
			u8 analog_r;
		};
	} in;
};

inline volatile auto *SICHANNEL = (SIChannel*)0xCC006400;
inline volatile auto *SISR = (SIStatus*)0xCC006438;

extern "C" {

// mask bits 24-27: vblank copy, 28-31: enable
u32 SIDisablePolling(u32 mask);
u32 SIEnablePolling(u32 mask);

u32 SIGetType(u32 port);
u32 SITransfer(s32 chan, void *out, u32 out_len, void *in, u32 in_len, SICallback cb, u32 us_delay);

bool SIEnablePollingInterrupt(bool enable);

extern SIControl Si;

} // extern "C"
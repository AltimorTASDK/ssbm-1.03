#pragma once

#include <gctypes.h>

struct SIData {
	char pad000[0x04];
	struct {
		u32 : 6;
		u32 x : 10;
		u32 y : 8;
		u32 enable : 4;
		u32 vblank_copy : 4;
	} poll;
	char pad008[0x14 - 0x08];
};

extern "C" {

extern SIData Si;

}
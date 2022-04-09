#pragma once

#include "os/os.h"
#include <gctypes.h>

struct card_block {
	s32 attached;
	s32 result;
	u32 pad008;
	u32 sector_size;
	char pad010[0x108 - 0x10];
#ifndef NTSC100
	u32 cid;
	struct {
		u32 game;
		u16 company;
	} *gamecode;
#endif
};

extern "C" {

extern card_block cardmap[2];

void __CARDSetDiskID(const char gamecode[6]);

} // extern "C"
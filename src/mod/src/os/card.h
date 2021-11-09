#pragma once

#include <gctypes.h>

struct card_block {
	s32 attached;
	s32 result;
	u32 pad008;
	u32 sector_size;
	char pad010[0x110 - 0x10];
};

extern "C" card_block cardmap[2];
#pragma once

#include <gctypes.h>

struct card_block {
	s32 attached;
	s32 result;
	u32 pad008;
	u32 sector_size;
	char pad010[0x10C - 0x10];
	struct {
		u32 game;
		u16 company;
	} *gamecode;
};

extern "C" card_block cardmap[2];
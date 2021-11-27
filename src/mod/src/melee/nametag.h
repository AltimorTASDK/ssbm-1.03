#pragma once

#include <gctypes.h>

constexpr auto NAMETAG_MAX = 120;

struct NameTagEntry {
	char pad000[0x1A1];
	u8 rumble;
	char pad1A2[0x1A4 - 0x1A2];
};

extern "C" NameTagEntry *NameTag_GetEntry(s32 index);
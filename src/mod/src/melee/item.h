#pragma once

#include <gctypes.h>

struct Item {
	char pad000[0xDC8];
	struct {
		u8 flags1_80 : 1;
		u8 flags1_40 : 1;
		u8 flags1_20 : 1;
		u8 no_update : 1;
		u8 flags1_08 : 1;
		u8 flags1_04 : 1;
		u8 flags1_02 : 1;
		u8 flags1_01 : 1;
	};
};
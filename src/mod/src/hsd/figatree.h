#pragma once

#include <gctypes.h>

struct HSD_FigaTree {
	s32 type;
	u32 aobj_flags;
	f32 frames;
	void *counts;
	void *tracks;
};
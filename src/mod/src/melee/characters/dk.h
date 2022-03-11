#pragma once

#include "hsd/gobj.h"

enum ActionStateDK {
	AS_DK_ThrowFF = 361,
	AS_DK_ThrowFB,
	AS_DK_ThrowFHi,
	AS_DK_ThrowFLw,
};

extern "C" void AS_DK_ThrowFDecide(HSD_GObj *gobj, u32 state);
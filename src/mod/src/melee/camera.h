#pragma once

#include "hsd/gobj.h"

struct Camera {
	HSD_GObj *gobj;
	char pad004[0x3A8 - 0x004];
};

extern "C" Camera MainCamera;
#pragma once

#include "hsd/gobj.h"
#include "util/vector.h"
#include <gctypes.h>

struct Camera {
	HSD_GObj *gobj;
	char pad004[0x3A8 - 0x004];
};

struct CmSubject {
	CmSubject *next;
	CmSubject *prev;
	char pad008[0x1C - 0x08];
	vec3 pos;
	char pad028[0x3C - 0x28];
	f32 size;
	f32 bounds[4];
	char pad050[0x6C - 0x50];
};

extern "C" Camera MainCamera;
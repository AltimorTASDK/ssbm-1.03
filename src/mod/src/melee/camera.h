#pragma once

#include "hsd/gobj.h"
#include "util/vector.h"
#include <gctypes.h>

enum CameraMode {
	CameraMode_Normal = 0,
	CameraMode_Fixed = 4
};

struct CameraMovement;

struct Camera {
	HSD_GObj *gobj;
	u32 mode;
	char pad008[0x3A8 - 0x008];
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
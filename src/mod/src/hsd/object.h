#pragma once

#include <gctypes.h>

struct HSD_ClassInfo;

enum ObjType {
	ObjType_DObj = 3,
	ObjType_JObj = 6,
	ObjType_MObj = 8,
	ObjType_PObj = 9,
	ObjType_RObj = 10,
	ObjType_TObj = 11
};

enum ObjMask {
	ObjMask_DObj = 0x004,
	ObjMask_JObj = 0x020,
	ObjMask_MObj = 0x080,
	ObjMask_PObj = 0x100,
	ObjMask_RObj = 0x200,
	ObjMask_TObj = 0x400
};

struct HSD_Class {
	HSD_ClassInfo *class_info;
};

struct HSD_Obj {
	HSD_Class parent;
	s16 ref_count;
	s16 ref_count_individual;
};
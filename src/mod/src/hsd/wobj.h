#pragma once

#include "hsd/object.h"
#include "util/vector.h"

struct HSD_AObj;
struct HSD_RObj;
struct HSD_RObjDesc;

struct HSD_WObjDesc {
	char *class_name;
	vec3 pos;
	HSD_RObjDesc *robjdesc;
};

struct HSD_WObj {
	HSD_Obj base;
	u32 flags;
	vec3 pos;
	HSD_AObj *aobj;
	HSD_RObj *robj;
};
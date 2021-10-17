#pragma once

#include "hsd/object.h"
#include <gctypes.h>

struct HSD_AObj;
struct HSD_MObj;
struct HSD_MObjDesc;
struct HSD_PObj;
struct HSD_PObjDesc;

struct HSD_DObj {
	HSD_Class parent;
	HSD_DObj *next;
	HSD_MObj *mobj;
	HSD_PObj *pobj;
	HSD_AObj *aobj;
	u32 flags;
};

struct HSD_DObjDesc {
	char *class_name;
	HSD_DObjDesc *next;
	HSD_MObjDesc *mobjdesc;
	HSD_PObjDesc *pobjdesc;
};
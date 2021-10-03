#pragma once

#include "hsd/object.h"
#include "util/vector.h"
#include "util/matrix.h"
#include <gctypes.h>

struct HSD_AObj;
struct HSD_RObj;
struct HSD_JObjDesc;

struct HSD_JObj {
	HSD_Obj base;
	HSD_JObj *next_sibling; // Next bone with the same parent
	HSD_JObj *parent;
	HSD_JObj *first_child;
	u32 flags;
	union {
		struct _HSD_SList *ptcl;
		struct _HSD_DObj *dobj;
		struct _HSD_Spline *spline;
	} u;
	vec4 rotation;
	char pad0028[0x2C - 0x28];
	vec3 scale;
	vec3 position;
	matrix3x4 mtx;
	vec3 *pvec;
	matrix3x4 *vmtx;
	HSD_AObj *aobj;
	HSD_RObj *robj;
	HSD_JObjDesc *desc;
};

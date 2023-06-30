#pragma once

#include "hsd/object.h"
#include <cstddef>
#include <gctypes.h>

struct HSD_AObj;
struct HSD_MObj;
struct HSD_MObjDesc;
struct HSD_PObj;
struct HSD_PObjDesc;

enum DObjFlag {
	DOBJ_HIDDEN = (1 << 0)
};

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

template<size_t N>
inline HSD_DObj *HSD_DObjGetByIndex(HSD_DObj *dobj)
{
	if constexpr (N == 0)
		return dobj;
	else
		return HSD_DObjGetByIndex<N - 1>(dobj->next);
}
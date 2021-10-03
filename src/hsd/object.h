#pragma once

#include <gctypes.h>

struct HSD_ClassInfo;

struct HSD_Class {
	HSD_ClassInfo *class_info;
};

struct HSD_Obj {
	HSD_Class parent;
	s16 ref_count;
	s16 ref_count_individual;
};

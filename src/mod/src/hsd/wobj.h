#pragma once

#include "util/vector.h"

struct HSD_RObjDesc;

struct HSD_WObjDesc {
	char *class_name;
	vec3 pos;
	HSD_RObjDesc *robjdesc;
};
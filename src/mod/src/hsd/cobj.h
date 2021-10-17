#pragma once

#include "util/matrix.h"
#include <ogc/gx.h>

struct HSD_CObj;

extern "C" {

HSD_CObj *HSD_CObjGetCurrent();
void HSD_CObjGetViewingMtx(HSD_CObj *cobj, Mtx *out);

}
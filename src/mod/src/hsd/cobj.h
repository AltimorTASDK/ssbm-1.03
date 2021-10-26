#pragma once

#include "util/matrix.h"
#include <ogc/gx.h>

struct HSD_CObj;

extern "C" {

HSD_CObj *HSD_CObjGetCurrent();
bool HSD_CObjSetCurrent(HSD_CObj *cobj);
void HSD_CObjGetViewingMtx(const HSD_CObj *cobj, Mtx *out);

}
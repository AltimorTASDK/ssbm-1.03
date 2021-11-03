#pragma once

struct HSD_LObj;
struct HSD_LObjDesc;

extern "C" {

HSD_LObj *HSD_LObjLoadDescList(HSD_LObjDesc **descs);
	
}
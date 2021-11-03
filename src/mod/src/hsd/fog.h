#pragma once

struct HSD_Fog;
struct HSD_FogDesc;

extern "C" {
	
HSD_Fog *HSD_FogLoadDesc(const HSD_FogDesc *desc);

}
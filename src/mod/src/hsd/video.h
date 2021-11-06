#pragma once

#include <gctypes.h>

enum HSD_RenderPass {
	HSD_RP_SCREEN,
	HSD_RP_TOPHALF,
	HSD_RP_BOTTOMHALF,
	HSD_RP_OFFSCREEN,
	HSD_RP_NUM
};

enum HSD_VIXFBDrawDispStatus {
	HSD_VI_XFB_NONE     = 0,
	HSD_VI_XFB_FREE     = 2,
	HSD_VI_XFB_DRAWING  = 3,
	HSD_VI_XFB_WAITDONE = 4,
	HSD_VI_XFB_DRAWDONE = 5,
	HSD_VI_XFB_NEXT     = 6,
	HSD_VI_XFB_DISPLAY  = 7,
	HSD_VI_XFB_COPYEFB  = 8,
};

using VIRetraceCallback = void(u32 retrace_count);

struct VIAll {
	char pad000[0x58];
};

struct XFB {
	void *buffer;
	u32 status;
	VIAll vi_all;
};

struct HSD_VIInfo {
	char pad0000[0x1458];
	XFB xfb[3];
	char pad1578[0x15F8 - 0x1578];
};

extern "C" {
	
extern HSD_VIInfo HSD_VIData;
	
s32 HSD_VISearchXFBByStatus(u32 status);
void HSD_VICopyXFBASync(u32 pass);
VIRetraceCallback *HSD_VISetUserPreRetraceCallback(VIRetraceCallback *cb);
VIRetraceCallback *HSD_VISetUserPostRetraceCallback(VIRetraceCallback *cb);
	
}
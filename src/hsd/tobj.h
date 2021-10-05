#pragma once

#include <gctypes.h>

struct HSD_AObjDesc;

struct HSD_TlutDesc {
	void *lut;
	u32 fmt;
	u32 tlut_name;
	u32 n_entries;
};

struct HSD_ImageDesc {
	void *img_ptr;
	u16 width;
	u16 height;
	u32 format;
	u32 mipmap;
	f32 minLOD;
	f32 maxLOD;
};

struct HSD_TexAnim {
	HSD_TexAnim *next;
	u32 id;
	HSD_AObjDesc *aobjdesc;
	HSD_ImageDesc **imagetbl;
	HSD_TlutDesc **tluttbl;
	u16 n_imagetbl;
	u16 n_tluttbl;
};
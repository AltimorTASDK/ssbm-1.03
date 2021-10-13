#pragma once

#include "util/vector.h"
#include <gctypes.h>

struct HSD_AObjDesc;
struct HSD_TexLODDesc;
struct HSD_TObjTevDesc;

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
	f32 min_lod;
	f32 max_lod;
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

struct HSD_TObjDesc {
	char *class_name;
	HSD_TObjDesc *next;
	u32 id;
	u32 src;
	vec3 rotate;
	vec3 scale;
	vec3 translate;
	u32 wrap_s;
	u32 wrap_t;
	u8 repeat_s;
	u8 repeat_t;
	u32 blend_flags;
	f32 blending;
	u32 mag_filt;
	HSD_ImageDesc *imagedesc;
	HSD_TlutDesc *tlut_desc;
	HSD_TexLODDesc *lod;
	HSD_TObjTevDesc *tev;
};
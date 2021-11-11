#pragma once

#include "hsd/object.h"
#include "util/matrix.h"
#include "util/vector.h"
#include <gctypes.h>

struct HSD_AObj;
struct HSD_AObjDesc;
struct HSD_TexLODDesc;

struct HSD_Tlut {
	void *lut;
	u32 fmt;
	u32 tlut_name;
	u32 n_entries;
};

using HSD_TlutDesc = HSD_Tlut;

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

struct HSD_TObjTev {
	u8 color_op;
	u8 alpha_op;
	u8 color_bias;
	u8 alpha_bias;
	u8 color_scale;
	u8 alpha_scale;
	u8 color_clamp;
	u8 alpha_clamp;
	u8 color_a;
	u8 color_b;
	u8 color_c;
	u8 color_d;
	u8 alpha_a;
	u8 alpha_b;
	u8 alpha_c;
	u8 alpha_d;
	color_rgba constant;
	color_rgba tev0;
	color_rgba tev1;
	u32 active;
};

using HSD_TObjTevDesc = HSD_TObjTev;

struct HSD_TObj {
	HSD_Obj base;
	HSD_TObj *next;
	u32 id;
	u32 src;
	u32 mtxid;
	vec4 rotate;
	vec3 scale;
	vec3 translate;
	u32 wrap_s;
	u32 wrap_t;
	u8 repeat_s;
	u8 repeat_t;
	u16 anim_id;
	u32 flags;
	f32 blending;
	u32 mag_filt;
	HSD_ImageDesc *imagedesc;
	HSD_Tlut *tlut;
	HSD_TexLODDesc *lod;
	HSD_AObj *aobj;
	HSD_ImageDesc **imagetbl;
	HSD_Tlut **tluttbl;
	u8 tlut_no;
	matrix3x4 mtx;
	u32 coord;
	HSD_TObjTev *tev;
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
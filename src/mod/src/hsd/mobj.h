#pragma once

#include "hsd/object.h"
#include "util/vector.h"

struct HSD_AObj;
struct HSD_AObjDesc;
struct HSD_RenderAnim;
struct HSD_TexAnim;
struct HSD_TObj;
struct HSD_TObjDesc;
struct HSD_TExpTevDesc;
struct HSD_TExp;

enum RenderMode {
	RENDER_SHADOW = (1 << 26)
};

struct HSD_ChanAnim {
	HSD_ChanAnim *next;
	HSD_AObjDesc *aobjdesc;
};

struct HSD_TevRegAnim {
	HSD_TevRegAnim *next;
	HSD_AObjDesc *aobjdesc;
};

struct HSD_RenderAnim {
	HSD_ChanAnim *chananim;
	HSD_TevRegAnim *reganim;
};

struct HSD_MatAnim {
	HSD_MatAnim *next;
	HSD_AObjDesc *aobjdesc;
	HSD_TexAnim *texanim;
	HSD_RenderAnim *renderanim;
};

struct HSD_MatAnimJoint {
	HSD_MatAnimJoint *child;
	HSD_MatAnimJoint *next;
	HSD_MatAnim *matanim;
};

struct HSD_Material {
	color_rgba ambient;
	color_rgba diffuse;
	color_rgba specular;
	f32 alpha;
	f32 shininess;
};

struct HSD_PEDesc {
	u8 flags;
	u8 ref0;
	u8 ref1;
	u8 dst_alpha;
	u8 type;
	u8 src_factor;
	u8 dst_factor;
	u8 logic_op;
	u8 z_comp;
	u8 alpha_comp0;
	u8 alpha_op;
	u8 alpha_comp1;
};

struct HSD_MObj {
	HSD_Class base;
	u32 rendermode;
	HSD_TObj *tobj;
	HSD_Material *mat;
	HSD_PEDesc *pedesc;
	HSD_AObj *aobj;
	HSD_TExpTevDesc *tevdesc;
	HSD_TExp *texp;
	HSD_TObj *tobj_toon;
	HSD_TObj *tobj_gradation;
	HSD_TObj *tobj_backlight;
	f32 z_offset;
};

struct HSD_MObjDesc {
	char *class_name;
	u32 rendermode;
	HSD_TObjDesc *texdesc;
	HSD_Material *mat;
	void *renderdesc;
	HSD_PEDesc *pedesc;
};
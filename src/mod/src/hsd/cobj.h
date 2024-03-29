#pragma once

#include "hsd/object.h"
#include "hsd/wobj.h"
#include "qol/widescreen.h"
#include "util/matrix.h"
#include "util/vector.h"
#include <ogc/gx.h>

enum ProjType {
	ProjType_Perspective = 1,
	ProjType_Frustum = 2,
	ProjType_Ortho = 3
};

struct HSD_AObj;
struct HSD_WObj;
struct HSD_WObjDesc;

struct HSD_CObj {
	HSD_Obj base;
	u32 flags;
	f32 viewport_left;
	f32 viewport_right;
	f32 viewport_top;
	f32 viewport_bottom;
	u16 scissor_left;
	u16 scissor_right;
	u16 scissor_top;
	u16 scissor_bottom;
	HSD_WObj *eye_position;
	HSD_WObj *interest;
	union {
		f32 roll;
		vec3 up;
	};
	f32 near;
	f32 far;
	union {
		struct {
			f32 fov;
			f32 aspect;
		} perspective;
		struct {
			f32 top;
			f32 bottom;
			f32 left;
			f32 right;
		} frustum, ortho;
	} u;
	u8 projection_type;
	matrix3x4 view_mtx;
	HSD_AObj *aobj;
	matrix3x4 *proj_mtx;
};

struct HSD_CObjDesc {
	const char *class_name;
	u16 flags;
	u16 projection_type;
	u16 viewport_left;
	u16 viewport_right;
	u16 viewport_top;
	u16 viewport_bottom;
	u16 scissor_left;
	u16 scissor_right;
	u16 scissor_top;
	u16 scissor_bottom;
	const HSD_WObjDesc *eye_position;
	const HSD_WObjDesc *interest;
	f32 roll;
	const vec3 *up;
	f32 near;
	f32 far;
	union {
		struct {
			f32 fov;
			f32 aspect;
		} perspective;
		struct {
			f32 top;
			f32 bottom;
			f32 left;
			f32 right;
		} frustum, ortho;
	} u;
};

// Fullscreen 2D cobjdescs
inline constexpr auto canvas_eye      = HSD_WObjDesc { .pos = { 0, 0, 1 } };
inline constexpr auto canvas_interest = HSD_WObjDesc { .pos = { 0, 0, 0 } };

inline constinit auto canvas_cobjdesc = HSD_CObjDesc {
	.projection_type = ProjType_Ortho,
	.viewport_right  = 640,
	.viewport_bottom = 480,
	.scissor_right   = 640,
	.scissor_bottom  = 480,
	.eye_position    = &canvas_eye,
	.interest        = &canvas_interest,
	.far             = 65535,
	.u = {
		.ortho   = { 0, -480, 0, 640 }
	}
};

inline constinit auto canvas_cobjdesc_wide = HSD_CObjDesc {
	.projection_type = ProjType_Ortho,
	.viewport_right  = 640,
	.viewport_bottom = 480,
	.scissor_right   = 640,
	.scissor_bottom  = 480,
	.eye_position    = &canvas_eye,
	.interest        = &canvas_interest,
	.far             = 65535,
	.u = {
		.ortho   = { 0, -480, ortho_left_wide, ortho_right_wide }
	}
};

extern "C" {

HSD_CObj *HSD_CObjLoadDesc(const HSD_CObjDesc *desc);

HSD_CObj *HSD_CObjGetCurrent();
bool HSD_CObjSetCurrent(HSD_CObj *cobj);
bool HSD_CObjSetEyePosition(HSD_CObj *cobj, const vec3 &position);
void HSD_CObjGetScissor(const HSD_CObj *cobj, u16 *scissor);
void HSD_CObjGetViewingMtx(const HSD_CObj *cobj, Mtx out);

void HSD_CObjTransform(HSD_CObj *cobj, const vec3 &in, vec3 *out, bool recalc_mtx);

} // extern "C"
#include "hsd/cobj.h"
#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "hsd/tobj.h"
#include "melee/camera.h"
#include "rules/values.h"
#include "util/math.h"
#include "util/vector.h"
#include <cmath>
#include <gctypes.h>

struct OffScreenBubbleData {
	HSD_GObj *gobj;
	HSD_JObj *jobj;
	HSD_ImageDesc *imagedesc;
	struct {
		u8 flags_80 : 1;
		u8 flags_40 : 1;
		u8 direction : 6;
	};
};

struct TrainingMenuData {
	char pad000[0xA4];
	HSD_GObj *gobj;
	HSD_JObj *jobj_tree[39];
	s32 anim_frames[39];
	char pad1E0[0x204 - 0x1E0];
};

extern "C" HSD_CObjDesc ScreenFlashCObjDesc;

constexpr auto aspect_ratio_vanilla = 913.f / 750.f;
constexpr auto aspect_ratio_factor = 320.f / 219.f;
constexpr auto aspect_ratio_wide = aspect_ratio_vanilla * aspect_ratio_factor;

extern "C" int orig_CObjLoad(HSD_CObj *cobj, HSD_CObjDesc *desc);
extern "C" int hook_CObjLoad(HSD_CObj *cobj, HSD_CObjDesc *desc)
{
	const auto result = orig_CObjLoad(cobj, desc);
	
	// Don't prevent screen flash from covering screen
	if (desc == &ScreenFlashCObjDesc)
		return result;
	
	if (is_widescreen() && cobj->projection_type == ProjType_Perspective)
		cobj->perspective.aspect *= aspect_ratio_factor;
	
	return result;
}

extern "C" bool orig_WorldToScreen(const vec3 &in, vec2 *out);
extern "C" bool hook_WorldToScreen(const vec3 &in, vec2 *out)
{
	auto *cobj = MainCamera.gobj->get_hsd_obj<HSD_CObj>();
	
	// Perform off screen check with the original aspect ratio
	const auto old_aspect = cobj->perspective.aspect;
	cobj->perspective.aspect = aspect_ratio_vanilla;
	const auto on_screen = orig_WorldToScreen(in, out);
	cobj->perspective.aspect = old_aspect;

	orig_WorldToScreen(in, out);
	return on_screen;
}

extern "C" void orig_OffScreenBubble_GetPosition(OffScreenBubbleData *data,
                                                 const vec2 &in, vec2 *out);
extern "C" void hook_OffScreenBubble_GetPosition(OffScreenBubbleData *data,
                                                 const vec2 &in, vec2 *out)
{
	if (!is_widescreen())
		return orig_OffScreenBubble_GetPosition(data, in, out);
		
	constexpr auto bound_x = 320.f - 67.3f;
	constexpr auto bound_y = 240.f - 77.3f;
	constexpr auto bound_ratio = bound_y / bound_x;
	
	// Scale squashed X coord to match true aspect ratio
	const auto scaled = vec2 { in.x * aspect_ratio_factor, in.y };
	
	const auto ratio = scaled.y / scaled.x;
	if (ratio > bound_ratio || ratio < -bound_ratio) {
		// Top/bottom edges
		out->y = std::copysign(bound_y, scaled.y);
		out->x = clamp(out->y / ratio, -bound_x, bound_x);
		data->direction = scaled.y > 0.f ? 1 : 3;
	} else {
		// Left/right edges
		out->x = std::copysign(bound_x, scaled.x);
		out->y = clamp(out->x * ratio, -bound_y, bound_y);
		data->direction = scaled.x > 0.f ? 4 : 2;
	}
}
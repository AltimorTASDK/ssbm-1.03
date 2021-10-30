#include "hsd/cobj.h"
#include "melee/camera.h"
#include "rules/values.h"
#include "util/vector.h"
#include <gctypes.h>

constexpr auto aspect_ratio_vanilla = 913.f / 750.f;
constexpr auto aspect_ratio_factor = 320.f / 219.f;
constexpr auto aspect_ratio_wide = aspect_ratio_vanilla * aspect_ratio_factor;

extern "C" int orig_CObjLoad(HSD_CObj *cobj, HSD_CObjDesc *desc);
extern "C" int hook_CObjLoad(HSD_CObj *cobj, HSD_CObjDesc *desc)
{
	const auto result = orig_CObjLoad(cobj, desc);
	
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
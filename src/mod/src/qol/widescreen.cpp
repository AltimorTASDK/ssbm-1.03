#include "hsd/cobj.h"
#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "hsd/tobj.h"
#include "melee/camera.h"
#include "melee/text.h"
#include "rules/values.h"
#include "util/math.h"
#include "util/vector.h"
#include <cmath>
#include <gctypes.h>

enum TrainingMenuJoint {
	TrainingMenuJoint_ItemText = 1,
	TrainingMenuJoint_Banner = 2,
	TrainingMenuJoint_RightPanel = 3,
	TrainingMenuJoint_LeftPanel = 22
};

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
	s32 speed;
	s32 item;
	s32 cpu_count;
	s32 cpu_action;
	s32 cpu_damage;
	s32 pad1F4;
	s32 camera_type;
	Text *item_text;
};

extern "C" HSD_CObjDesc ScreenFlashCObjDesc;

extern "C" HSD_GObj *NameTagGObjs[6];

extern "C" TrainingMenuData TrainingMenu;

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

extern "C" bool orig_CmSubject_WorldToScreen(const CmSubject *subject, vec2 *out);
extern "C" bool hook_CmSubject_WorldToScreen(const CmSubject *subject, vec2 *out)
{
	auto *cobj = MainCamera.gobj->get_hsd_obj<HSD_CObj>();
	
	// Perform off screen check with the original aspect ratio
	const auto old_aspect = cobj->perspective.aspect;
	cobj->perspective.aspect = aspect_ratio_vanilla;
	const auto on_screen = orig_CmSubject_WorldToScreen(subject, out);
	cobj->perspective.aspect = old_aspect;

	orig_CmSubject_WorldToScreen(subject, out);
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
extern "C" void orig_TrainingMenu_Create();
extern "C" void hook_TrainingMenu_Create()
{
	orig_TrainingMenu_Create();
	
	if (is_widescreen()) {
		TrainingMenu.item_text->aspect.x /= aspect_ratio_factor;
		TrainingMenu.item_text->default_scale.x /= aspect_ratio_factor;
	}
}

extern "C" void orig_TrainingMenu_Think();
extern "C" void hook_TrainingMenu_Think()
{
	orig_TrainingMenu_Think();
	
	if (!is_widescreen())
		return;

	// Calc offset based on the X coord at the edge of the camera
	constexpr auto screen_edge = 64.f * (float)tan(deg_to_rad(41.539f / 2.f)) * (73.f / 60.f);
	constexpr auto offset = screen_edge * (aspect_ratio_factor - 1.f);

	for (auto i = 0; i < 39; i++) {
		auto *jobj = TrainingMenu.jobj_tree[i];

		// Recalc matrices so offsets propagate
		HSD_JObjSetupMatrix(jobj);

		switch (i) {
		case TrainingMenuJoint_ItemText:
		case TrainingMenuJoint_Banner:
		case TrainingMenuJoint_LeftPanel:
			jobj->mtx.get<0, 3>() -= offset;
			break;
		case TrainingMenuJoint_RightPanel:
			jobj->mtx.get<0, 3>() += offset;
			break;
		}
	}
	
	// Move item text to accomodate stretched viewport
	TrainingMenu.item_text->trans.x /= aspect_ratio_factor;
}

extern "C" void orig_NameTag_SetupForPlayer(u32 port);
extern "C" void hook_NameTag_SetupForPlayer(u32 port)
{
	orig_NameTag_SetupForPlayer(port);
	
	// Scale nametag background
	if (is_widescreen())
		NameTagGObjs[port]->get_hsd_obj<HSD_JObj>()->scale.x /= aspect_ratio_factor;
}

extern "C" void orig_Text_UpdateSubtextSize(Text *text, int subtext, float x, float y);
extern "C" void hook_Text_UpdateSubtextSize(Text *text, int subtext, float x, float y)
{
	// Scale nametags
	if (is_widescreen() && text == NameTagText)
		x /= aspect_ratio_factor;

	orig_Text_UpdateSubtextSize(text, subtext, x, y);
}

extern "C" void orig_DevelopText_Draw(DevText *text);
extern "C" void hook_DevelopText_Draw(DevText *text)
{
	if (!is_widescreen())
		return orig_DevelopText_Draw(text);

	// Scale develop text
	const auto old_scale_x = text->scale.x;
	text->scale.x /= aspect_ratio_factor;
	orig_DevelopText_Draw(text);
	text->scale.x = old_scale_x;
}
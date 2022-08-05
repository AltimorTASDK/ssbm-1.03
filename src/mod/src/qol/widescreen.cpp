#include "hsd/cobj.h"
#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "hsd/tobj.h"
#include "hsd/video.h"
#include "os/gx.h"
#include "melee/camera.h"
#include "melee/text.h"
#include "qol/widescreen.h"
#include "rules/values.h"
#include "util/math.h"
#include "util/vector.h"
#include "util/draw/render.h"
#include "util/draw/texture.h"
#include <cmath>
#include <gctypes.h>

enum TrainingMenuJoint {
	TrainingMenuJoint_ItemText = 1,
	TrainingMenuJoint_Banner = 2,
	TrainingMenuJoint_RightPanel = 3,
	TrainingMenuJoint_LeftPanel = 22
};

struct BubbleData {
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

extern "C" int orig_CObjLoad(HSD_CObj *cobj, HSD_CObjDesc *desc);
extern "C" int hook_CObjLoad(HSD_CObj *cobj, HSD_CObjDesc *desc)
{
	const auto result = orig_CObjLoad(cobj, desc);

	// Don't prevent screen flash from covering screen
	if (desc == &ScreenFlashCObjDesc)
		return result;

	if (is_widescreen() && cobj->projection_type == ProjType_Perspective)
		cobj->u.perspective.aspect *= aspect_ratio_factor;

	return result;
}

extern "C" bool orig_CmSubject_WorldToScreen(const CmSubject *subject, vec2 *out);
extern "C" bool hook_CmSubject_WorldToScreen(const CmSubject *subject, vec2 *out)
{
	auto *cobj = MainCamera.gobj->get_hsd_obj<HSD_CObj>();

	// Perform off screen check with the original aspect ratio
	const auto old_aspect = cobj->u.perspective.aspect;
	cobj->u.perspective.aspect = aspect_ratio_vanilla;
	const auto on_screen = orig_CmSubject_WorldToScreen(subject, out);
	cobj->u.perspective.aspect = old_aspect;

	orig_CmSubject_WorldToScreen(subject, out);
	return on_screen;
}

extern "C" void orig_Bubble_GetPosition(BubbleData *data, const vec2 &in, vec2 *out);
extern "C" void hook_Bubble_GetPosition(BubbleData *data, const vec2 &in, vec2 *out)
{
	// Scale squashed X coord to match true aspect ratio
	if (is_widescreen())
		orig_Bubble_GetPosition(data, in * vec2(aspect_ratio_factor, 1.f), out);
	else
		orig_Bubble_GetPosition(data, in, out);
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
	constexpr auto screen_edge = 64.f * (float)tan(0.725f / 2.f) * (73.f / 60.f);
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

static void apply_crop(const HSD_VIStatus *vi, void *buffer)
{
	if (!is_widescreen_crop())
		return;

	// Don't crash from Hannes Mann lag reduction
	if (buffer == nullptr)
		return;

	// Downscale EFB into 73:60 region on 16:9 display
	constexpr auto crop_width = (u16)(640.f / aspect_ratio_factor + .5f);
	constexpr auto crop_left  = (u16)((640 - crop_width) / 2);
	constexpr auto crop_right = crop_left + crop_width;

	// Copy in two halves so the XFB buffer can be reused as 32bpp
	const auto src_height = (u16)(vi->rmode.efbHeight / 2);
	const auto dst_height = (u16)(vi->rmode.xfbHeight / 2);

	const auto xfb_texture = texture(buffer, vi->rmode.fbWidth, dst_height, GX_TF_RGBA8);

	auto &rs = render_state::get();
	rs.reset_2d();

	GX_SetColorUpdate(GX_TRUE);
	GX_SetZMode(GX_FALSE, GX_NEVER, GX_FALSE);
	GX_SetCopyFilter(GX_FALSE, nullptr, GX_FALSE, nullptr);

	// Draw top half
	GX_SetTexCopySrc(0, 0, vi->rmode.fbWidth, src_height);
	GX_SetTexCopyDst(vi->rmode.fbWidth, dst_height, GX_TF_RGBA8, GX_FALSE);
	GX_CopyTex(buffer, GX_FALSE);
	GX_PixModeSync();
	GX_InvalidateTexAll();
	rs.fill_rect({crop_left, 0, 0}, {crop_width, 240}, xfb_texture, {0, 0}, {1, 1});

	xfb_texture.invalidate();

	// Draw bottom half
	GX_SetTexCopySrc(0, src_height, vi->rmode.fbWidth, src_height);
	GX_SetTexCopyDst(vi->rmode.fbWidth, dst_height, GX_TF_RGBA8, GX_FALSE);
	GX_CopyTex(buffer, GX_FALSE);
	GX_PixModeSync();
	GX_InvalidateTexAll();
	rs.fill_rect({crop_left, 240, 0}, {crop_width, 240}, xfb_texture, {0, 0}, {1, 1});

	// Draw black bars
	GX_SetNumTexGens(0);
	GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);

	rs.fill_rect({0,          0, 0}, {crop_left, 480}, color_rgba::hex(0x000000FF));
	rs.fill_rect({crop_right, 0, 0}, {crop_left, 480}, color_rgba::hex(0x000000FF));
}

extern "C" void orig_HSD_VICopyEFB2XFBPtr(HSD_VIStatus *vi, void *buffer, u32 rpass);
extern "C" void hook_HSD_VICopyEFB2XFBPtr(HSD_VIStatus *vi, void *buffer, u32 rpass)
{
	apply_crop(vi, buffer);
	orig_HSD_VICopyEFB2XFBPtr(vi, buffer, rpass);
}
#include "hsd/cobj.h"
#include "hsd/dobj.h"
#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "hsd/memory.h"
#include "hsd/wobj.h"
#include "melee/camera.h"
#include "melee/debug.h"
#include "melee/text.h"
#include "qol/widescreen.h"
#include "rules/values.h"
#include "util/patch_list.h"
#include "util/math.h"
#include "util/mempool.h"
#include "util/texture_swap.h"
#include "util/draw/render.h"
#include <array>

struct LanguageMenuData {
	u8 setting;
	u8 old_setting;
	bool ready;
	Text *text;
};

extern "C" HSD_GObj *LanguageMenuGObj;

extern "C" void Menu_LanguageMenuInput(HSD_GObj *gobj);

constexpr auto TEXT_WIDTH  = 16;
constexpr auto TEXT_HEIGHT = 5;

constexpr auto TEXT_SCALE_X = 12;
constexpr auto TEXT_SCALE_Y = 18;

// Create the illusion of the same coordinate space + Z offset as the existing UI
constexpr auto TEXT_COORD_SCALE = 21.f;

constexpr auto TEXT_GAP       = (int)(4 * TEXT_COORD_SCALE);
constexpr auto TEXT_MASTER_X  = 0;
constexpr auto TEXT_DEVELOP_X = TEXT_MASTER_X + TEXT_WIDTH * TEXT_SCALE_X + TEXT_GAP;

// Center the camera in the gap between the text
constexpr auto TEXT_MASTER_RIGHT = TEXT_MASTER_X + TEXT_WIDTH * TEXT_SCALE_X;
constexpr auto TEXT_DEVELOP_LEFT = TEXT_DEVELOP_X - 8;

constexpr auto CAMERA_X = (TEXT_MASTER_RIGHT + TEXT_DEVELOP_LEFT) / 2.f;
constexpr auto CAMERA_Y = 80.f;
constexpr auto CAMERA_Z = 32.5f * TEXT_COORD_SCALE;

constexpr auto INTEREST_Z = -18.5f * TEXT_COORD_SCALE;

constexpr auto text_eye      = HSD_WObjDesc { .pos = { CAMERA_X, CAMERA_Y, CAMERA_Z   } };
constexpr auto text_interest = HSD_WObjDesc { .pos = { CAMERA_X, CAMERA_Y, INTEREST_Z } };

static constinit auto text_cobjdesc = HSD_CObjDesc {
	.projection_type = ProjType_Perspective,
	.viewport_right  = 640,
	.viewport_bottom = 480,
	.scissor_right   = 640,
	.scissor_bottom  = 480,
	.eye_position    = &text_eye,
	.interest        = &text_interest,
	.roll            = math::pi,
	.far             = 65535,
	.u = {
		.perspective = {
			.fov    = rad_to_deg(.725f),
			.aspect = -4.f/3.f
		}
	}
};

static mempool pool;
static DevText *text_master;
static DevText *text_develop;
static HSD_GObj *text_camera;
static char text_master_buf[TEXT_WIDTH * TEXT_HEIGHT * 2];
static char text_develop_buf[TEXT_WIDTH * TEXT_HEIGHT * 2];

PATCH_LIST(
	// Return to Vs Menu
	// li r3, MenuType_VsMode
	std::pair { Menu_LanguageMenuInput+0x8C, 0x38600002u },
	// li r4, VsMenu_DebugMenu
	std::pair { Menu_LanguageMenuInput+0x90, 0x38800003u }
);

static void pool_free(void *data)
{
	HSD_Free(data); // Default free gobj data

	if (pool.dec_ref() == 0) {
		DevelopText_Remove(&text_master);
		DevelopText_Remove(&text_develop);
		GObj_Free(text_camera);
		text_camera = nullptr;
	}
}

extern "C" void orig_Menu_SetupLanguageMenu(u8 state);
extern "C" void hook_Menu_SetupLanguageMenu(u8 state)
{
	orig_Menu_SetupLanguageMenu(state);

	float frame;
	auto *data = LanguageMenuGObj->get<LanguageMenuData>();
	auto *jobj = LanguageMenuGObj->get_hsd_obj<HSD_JObj>()->child;

	// Set the highlighted option based on DbLevel
	switch (DbLevel) {
	case DbLKind_Develop:
		data->setting = 1;
		data->old_setting = 1;
		frame = 0.f;
		break;
	default:
		data->setting = 0;
		data->old_setting = 0;
		frame = 1.f;
		break;
	}

	HSD_JObjReqAnimAll(jobj, frame);
	HSD_JObjStopAnimByTypeAndFlags(jobj, 0xFF, ObjMask_JObj);
	HSD_JObjAnimAll(jobj);
	HSD_JObjReqAnimAll(jobj, 0.f);
	HSD_JObjStopAnimByTypeAndFlags(jobj, 0xFF, ObjMask_MObj);
	HSD_JObjAnimAll(jobj);

	// Hide flags
	HSD_DObjGetByIndex<2>(jobj->u.dobj)->flags |= DOBJ_HIDDEN;
	HSD_DObjGetByIndex<3>(jobj->u.dobj)->flags |= DOBJ_HIDDEN;
	HSD_DObjGetByIndex<6>(jobj->u.dobj)->flags |= DOBJ_HIDDEN;
	HSD_DObjGetByIndex<7>(jobj->u.dobj)->flags |= DOBJ_HIDDEN;
	HSD_DObjGetByIndex<8>(jobj->u.dobj)->flags |= DOBJ_HIDDEN;

	// Free assets on menu exit
	LanguageMenuGObj->user_data_remove_func = pool_free;

	if (pool.inc_ref() != 0)
		return;

	text_master = DevelopText_Create(0x6A, TEXT_MASTER_X, 0,
	                                 TEXT_WIDTH, TEXT_HEIGHT, text_master_buf);
	DevelopText_HideCursor(text_master);
	//DevelopText_SetBGColor(text_master, color_rgba::hex(0x40508080u));
	DevelopText_SetBGColor(text_master, color_rgba::hex(0x40508080u));
	DevelopText_SetTextColor(text_master, color_rgba::hex(0xFF8020FFu));
	DevelopText_SetScale(text_master, TEXT_SCALE_X, TEXT_SCALE_Y);
	DevelopText_SetCursor(text_master, 0, TEXT_HEIGHT / 2);
	DevelopText_Print(text_master, "DbLevel: Master");
	DevelopText_Show(nullptr, text_master);

	text_develop = DevelopText_Create(0x6B, TEXT_DEVELOP_X, 0,
	                                  TEXT_WIDTH, TEXT_HEIGHT, text_develop_buf);
	DevelopText_HideCursor(text_develop);
	DevelopText_SetBGColor(text_develop, color_rgba::hex(0x40508080u));
	//DevelopText_SetTextColor(text_develop, color_rgba::hex(0xFF8020FFu));
	DevelopText_SetTextColor(text_develop, color_rgba::hex(0xE2E2E2FFu));
	DevelopText_SetScale(text_develop, TEXT_SCALE_X, TEXT_SCALE_Y);
	DevelopText_SetCursor(text_develop, 0, TEXT_HEIGHT / 2);
	DevelopText_Print(text_develop, "DbLevel: Develop");
	DevelopText_Show(nullptr, text_develop);

	text_camera = GObj_Create(GOBJ_CLASS_CAMERA, GOBJ_PLINK_MENU_CAMERA, 0);
	GObj_InitKindObj(text_camera, GOBJ_KIND_CAMERA, HSD_CObjLoadDesc(&text_cobjdesc));
}

extern "C" void orig_Menu_UpdateCStickRotation(HSD_GObj *gobj);
extern "C" void hook_Menu_UpdateCStickRotation(HSD_GObj *gobj)
{
	orig_Menu_UpdateCStickRotation(gobj);

	if (text_camera == nullptr)
		return;

	constexpr auto scale = vec3(1, -1, 1) * TEXT_COORD_SCALE;
	constexpr auto offset = vec3(CAMERA_X, CAMERA_Y, INTEREST_Z);

	auto *menu_cobj = gobj->get_hsd_obj<HSD_CObj>();
	auto *text_cobj = text_camera->get_hsd_obj<HSD_CObj>();
	const auto eye_pos = menu_cobj->eye_position->pos;
	HSD_CObjSetEyePosition(text_cobj, eye_pos * scale + offset);
}

extern "C" void orig_DevelopText_Draw(DevText *text);
extern "C" void hook_DevelopText_Draw(DevText *text)
{
	if (text != text_master && text != text_develop) {
		if (!is_widescreen())
			return orig_DevelopText_Draw(text);

		// Apply widescreen scaling
		const auto old_scale_x = text->scale.x;
		text->scale.x /= aspect_ratio_factor;
		orig_DevelopText_Draw(text);
		text->scale.x = old_scale_x;

		return;
	}

	// Use custom camera for debug mode text
	auto *cobj = text_camera->get_hsd_obj<HSD_CObj>();
	auto *jobj = LanguageMenuGObj->get_hsd_obj<HSD_JObj>()->child;

	if (jobj->scale.x < .01f)
		return;

	// Scale with menu open animation
	cobj->u.perspective.aspect = -4.f/3.f / jobj->scale.x;

	auto *old_cobj = HSD_CObjGetCurrent();
	HSD_CObjSetCurrent(cobj);
	orig_DevelopText_Draw(text);
	HSD_CObjSetCurrent(old_cobj);
}
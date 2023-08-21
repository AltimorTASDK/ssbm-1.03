#include "hsd/cobj.h"
#include "hsd/dobj.h"
#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "hsd/mobj.h"
#include "hsd/tobj.h"
#include "hsd/memory.h"
#include "hsd/wobj.h"
#include "melee/camera.h"
#include "melee/debug.h"
#include "melee/menu.h"
#include "melee/text.h"
#include "qol/widescreen.h"
#include "rules/values.h"
#include "util/patch_list.h"
#include "util/math.h"
#include "util/mempool.h"
#include "util/texture_swap.h"
#include "util/melee/text_builder.h"
#include <array>

#include "resources/debug/text_develop.tex.h"
#include "resources/debug/text_master.tex.h"

struct LanguageMenuData {
	u8 setting;
	u8 old_setting;
	bool ready;
	Text *text;
};

extern "C" HSD_GObj *LanguageMenuGObj;

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

template<string_literal line1>
static constexpr auto make_description_text()
{
	return text_builder::build(
		text_builder::kern(),
		text_builder::center(),
		text_builder::color<170, 170, 170>(),
		text_builder::scale<179, 179>(),
		text_builder::type_speed<0, 0>(),
		text_builder::fit(),
		text_builder::text<line1>(),
		text_builder::end_fit(),
		text_builder::reset_scale(),
		text_builder::end_color());
}

template<string_literal line1, string_literal line2>
static constexpr auto make_description_text()
{
	return text_builder::build(
		text_builder::kern(),
		text_builder::center(),
		text_builder::color<170, 170, 170>(),
		text_builder::scale<179, 128>(),
		text_builder::offset<0, -20>(),
		text_builder::br(),
		text_builder::type_speed<0, 0>(),
		text_builder::fit(),
		text_builder::text<line1>(),
		text_builder::end_fit(),
		text_builder::br(),
		text_builder::type_speed<0, 0>(),
		text_builder::fit(),
		text_builder::text<line2>(),
		text_builder::end_fit(),
		text_builder::reset_scale(),
		text_builder::end_color());
}

constexpr auto bottom_text = make_description_text<
	"Choose a debug mode and press",
	"the A Button to confirm.">();

static mempool pool;
static DevText *text_master;
static DevText *text_develop;
static HSD_GObj *text_camera;
static char text_master_buf[TEXT_WIDTH * TEXT_HEIGHT * 2];
static char text_develop_buf[TEXT_WIDTH * TEXT_HEIGHT * 2];

static void pool_free(void *data)
{
	HSD_Free(data); // Default free gobj data

	if (pool.dec_ref() == 0) {
		DevelopText_Remove(&text_master);
		DevelopText_Remove(&text_develop);
		GObj_Free(text_camera);
		text_master = nullptr;
		text_develop = nullptr;
		text_camera = nullptr;
	}
}

static void update_selection(bool joint_anim)
{
	auto *data = LanguageMenuGObj->get<LanguageMenuData>();
	auto *jobj = LanguageMenuGObj->get_hsd_obj<HSD_JObj>()->child;

	// Set matanim for selection
	HSD_JObjReqAnimAll(jobj, data->setting == 0 ? 1.f : 0.f);
	HSD_JObjStopAnimByTypeAndFlags(jobj, 0xFF, ObjMask_JObj);
	HSD_JObjAnimAll(jobj);

	if (joint_anim) {
		// Set joint anim
		HSD_JObjReqAnimAll(jobj, 0.f);
		HSD_JObjStopAnimByTypeAndFlags(jobj, 0xFF, ObjMask_MObj);
		HSD_JObjAnimAll(jobj);
	}

	if (data->setting == 0) {
		DevelopText_SetTextColor(text_master,  color_rgba::hex(0xFF8020FFu));
		DevelopText_SetTextColor(text_develop, color_rgba::hex(0xE2E2E2FFu));
	} else {
		DevelopText_SetTextColor(text_master,  color_rgba::hex(0xE2E2E2FFu));
		DevelopText_SetTextColor(text_develop, color_rgba::hex(0xFF8020FFu));
	}

}

extern "C" void orig_Menu_SetupLanguageMenu(u8 state);
extern "C" void hook_Menu_SetupLanguageMenu(u8 state)
{
	orig_Menu_SetupLanguageMenu(state);

	auto *data = LanguageMenuGObj->get<LanguageMenuData>();
	auto *jobj = LanguageMenuGObj->get_hsd_obj<HSD_JObj>()->child;

	data->text->data = bottom_text.data();

	// Set the highlighted option based on DbLevel
	data->setting = DbLevel == DbLKind_Develop ? 1 : 0;
	data->old_setting = data->setting;

	// Hide flags
	HSD_DObjGetByIndex<2>(jobj->u.dobj)->flags |= DOBJ_HIDDEN;
	HSD_DObjGetByIndex<3>(jobj->u.dobj)->flags |= DOBJ_HIDDEN;
	HSD_DObjGetByIndex<6>(jobj->u.dobj)->flags |= DOBJ_HIDDEN;
	HSD_DObjGetByIndex<7>(jobj->u.dobj)->flags |= DOBJ_HIDDEN;
	HSD_DObjGetByIndex<8>(jobj->u.dobj)->flags |= DOBJ_HIDDEN;

	// Free assets on menu exit
	LanguageMenuGObj->user_data_remove_func = pool_free;

	if (pool.inc_ref() != 0) {
		update_selection(true);
		return;
	}

	text_master = DevelopText_Create(0x6A, TEXT_MASTER_X, 0,
	                                 TEXT_WIDTH, TEXT_HEIGHT, text_master_buf);
	DevelopText_HideCursor(text_master);
	DevelopText_SetBGColor(text_master, color_rgba::hex(0x40508080u));
	DevelopText_SetScale(text_master, TEXT_SCALE_X, TEXT_SCALE_Y);
	DevelopText_SetCursor(text_master, 0, TEXT_HEIGHT / 2);
	DevelopText_Print(text_master, "DbLevel: Master");
	DevelopText_Show(nullptr, text_master);

	text_develop = DevelopText_Create(0x6B, TEXT_DEVELOP_X, 0,
	                                  TEXT_WIDTH, TEXT_HEIGHT, text_develop_buf);
	DevelopText_HideCursor(text_develop);
	DevelopText_SetBGColor(text_develop, color_rgba::hex(0x40508080u));
	DevelopText_SetScale(text_develop, TEXT_SCALE_X, TEXT_SCALE_Y);
	DevelopText_SetCursor(text_develop, 0, TEXT_HEIGHT / 2);
	DevelopText_Print(text_develop, "DbLevel: Develop");
	DevelopText_Show(nullptr, text_develop);

	text_camera = GObj_Create(GOBJ_CLASS_CAMERA, GOBJ_PLINK_MENU_CAMERA, 0);
	GObj_InitKindObj(text_camera, GOBJ_KIND_CAMERA, HSD_CObjLoadDesc(&text_cobjdesc));

	// Replace the EN/JP label textures
	auto *text_master = jobj->u.dobj->next->next->next->next;
	auto *text_develop = text_master->next;
	pool.add_texture_swap(text_master_tex_data, text_master->mobj->tobj->imagedesc);
	pool.add_texture_swap(text_develop_tex_data, text_develop->mobj->tobj->imagedesc);

	update_selection(true);
}

extern "C" void hook_Menu_LanguageMenuInput(HSD_GObj *gobj)
{
	auto *data = LanguageMenuGObj->get<LanguageMenuData>();
	const auto buttons = Menu_GetButtons(PORT_ALL);

	if (buttons & MenuButton_B) {
		// Exit without saving
		Menu_PlaySFX(MenuSFX_Back);
		IsEnteringMenu = false;
		Menu_MainMenuTransition(MenuType_VsMode, VsMenu_DebugMenu, MenuState_ExitTo);
		return;
	}

	if (buttons & (MenuButton_Left | MenuButton_Right)) {
		// Change setting
		Menu_PlaySFX(MenuSFX_Scroll);
		data->setting = !data->setting;
		update_selection(false);
	}

	if ((buttons & MenuButton_A) && data->setting != data->old_setting) {
		// Save DbLevel and exit
		DbLevel = data->setting == 0 ? DbLKind_Master : DbLKind_Develop;
		Menu_PlaySFX(MenuSFX_Activate);
		IsEnteringMenu = false;
		Menu_MainMenuTransition(MenuType_VsMode, VsMenu_DebugMenu, MenuState_ExitTo);
	}
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
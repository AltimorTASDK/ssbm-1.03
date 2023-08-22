#include "compat/20XX.h"
#include "hsd/aobj.h"
#include "hsd/archive.h"
#include "hsd/dobj.h"
#include "hsd/fobj.h"
#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "hsd/mobj.h"
#include "hsd/tobj.h"
#include "melee/menu.h"
#include "melee/scene.h"
#include "melee/text.h"
#include "menu/controls.h"
#include "rules/values.h"
#include "util/texture_swap.h"
#include "util/meta.h"
#include "util/melee/fobj_builder.h"
#include "util/melee/text_builder.h"
#include <gctypes.h>

#include "resources/portals/controls_portal.tex.h"
#include "resources/portals/controls_preview.tex.h"
#include "resources/portals/options_preview.tex.h"
#include "resources/portals/debug_menu.tex.h"
#include "resources/portals/debug_menu_preview.tex.h"
#include "resources/portals/manual.tex.h"
#ifdef TOURNAMENT
#include "resources/portals/te/manual_preview.tex.h"
#else
#include "resources/portals/le/manual_preview.tex.h"
#endif
#include "resources/debug/debug_header.tex.h"
#include "resources/manual/manual_header.tex.h"

enum MenuColor {
	MenuColor_Blue   = 0,
	MenuColor_Red    = 1,
	MenuColor_Green  = 2,
	MenuColor_Yellow = 3,
	MenuColor_Purple = 4,
};

struct MainMenuData {
	u8 menu_type;
	u8 selected;
	u8 state;
	HSD_JObj *jobj_tree[42];
	Text *description_text;
};

struct MainMenuEnterData {
	u8 menu_type;
	u8 selected;
	u8 pad002;
};

// Menu frame
extern "C" ArchiveModel MenMainPanel_Top;

extern "C" ArchiveModel MenMainCursor_Top;
extern "C" ArchiveModel MenMainConTop_Top;

extern "C" void VsMenu_Think();

template<string_literal line1, string_literal line2>
static constexpr auto make_description_text()
{
	return text_builder::build(
		text_builder::kern(),
		text_builder::left(),
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

constexpr auto options_menu_description = make_description_text<
	"Adjust rumble and sound.">();

constexpr auto debug_menu_description = make_description_text<
	"Select debug mode.">();

constexpr auto controls_description = make_description_text<
	"Remap buttons and change",
	"other controller settings.">();

constexpr auto controls_z_jump_description = make_description_text<
	"Remap buttons.">();

constexpr auto manual_description = make_description_text<
	"Read the 1.03 manual.">();

// red    1.0 0.6 0.6
// blue   0.3 1.0 1.0
// yellow 1.0 0.9 0.3
// green  0.5 1.0 0.5
// purple 0.8 0.5 0.9

constexpr auto preview_color_r = fobj_builder<HSD_A_FRAC_U16, 32768>::write_keys<
	HSD_A_OP_CON,
	{ 0.3f,  50 },    //    0
	{ 1.0f,  50 },    //   50
	{ 0.5f,  50 },    //  100
	{ 1.0f,  49 },    //  150 Options
	{ 0.8f, 201 },    //  199
	{ 0.3f, 300 },    //  400
	{ 1.0f,  50 },    //  700 Melee
	{ 0.8f,  50 },    //  750 Debug Menu
	{ 0.3f,  50 },    //  800 Controls
	{ 0.5f,  50 },    //  850 1.03 Manual
	{ 1.0f, 100 },    //  900
	{ 0.5f, 300 },    // 1000
	{ 1.0f, 300 },    // 1300
	{ 0.8f, 400 },    // 1600
	{ 0.3f, 400 },    // 2000
	{ 1.0f, 550 },    // 2400
	{ 0.8f, 550 },    // 2950
	{ 0.3f,  50 },    // 3500
	{ 0.8f, 450 }>(); // 3550

constexpr auto preview_color_g = fobj_builder<HSD_A_FRAC_U16, 32768>::write_keys<
	HSD_A_OP_CON,
	{ 1.0f,  50 },    //    0
	{ 0.6f,  50 },    //   50
	{ 1.0f,  50 },    //  100
	{ 0.9f,  49 },    //  150 Options
	{ 0.5f, 201 },    //  199
	{ 1.0f, 300 },    //  400
	{ 0.6f,  50 },    //  700 Melee
	{ 0.5f,  50 },    //  750 Debug Menu
	{ 1.0f,  50 },    //  800 Controls
	{ 1.0f,  50 },    //  850 1.03 Manual
	{ 0.6f, 100 },    //  900
	{ 1.0f, 300 },    // 1000
	{ 0.9f, 300 },    // 1300
	{ 0.5f, 400 },    // 1600
	{ 1.0f, 400 },    // 2000
	{ 0.6f, 550 },    // 2400
	{ 0.5f, 550 },    // 2950
	{ 1.0f,  50 },    // 3500
	{ 0.5f, 450 }>(); // 3550

constexpr auto preview_color_b = fobj_builder<HSD_A_FRAC_U16, 32768>::write_keys<
	HSD_A_OP_CON,
	{ 1.0f,  50 },    //    0
	{ 0.6f,  50 },    //   50
	{ 0.5f,  50 },    //  100
	{ 0.3f,  49 },    //  150 Options
	{ 0.9f, 201 },    //  199
	{ 1.0f, 300 },    //  400
	{ 0.6f,  50 },    //  700 Melee
	{ 0.9f,  50 },    //  750 Debug Menu
	{ 1.0f,  50 },    //  800 Controls
	{ 0.5f,  50 },    //  850 1.03 Manual
	{ 0.6f, 100 },    //  900
	{ 0.5f, 300 },    // 1000
	{ 0.3f, 300 },    // 1300
	{ 0.9f, 400 },    // 1600
	{ 1.0f, 400 },    // 2000
	{ 0.6f, 550 },    // 2400
	{ 0.9f, 550 },    // 2950
	{ 1.0f,  50 },    // 3500
	{ 0.9f, 450 }>(); // 3550

[[gnu::constructor]] static void set_preview_anims()
{
	// Swap around preview animations
	MenuTypeDataTable[MenuType_VsMode].preview_anims[VsMenu_Controls]  = { 800, 849, 820 };
	MenuTypeDataTable[MenuType_VsMode].preview_anims[VsMenu_Options]   = { 150, 199, 170 };
	MenuTypeDataTable[MenuType_VsMode].preview_anims[VsMenu_DebugMenu] = { 750, 799, 770 };
	MenuTypeDataTable[MenuType_VsMode].preview_anims[VsMenu_Manual]    = { 850, 899, 870 };
}

[[gnu::constructor]] static void restrict_options_menu()
{
	// Remove Screen Display, Language, and Erase Data portals
	MenuTypeDataTable[MenuType_Options].option_count = 2;
}

extern "C" u32 hook_Menu_GetLightColorIndex(u8 type, u16 index)
{
	switch (type) {
	case MenuType_VsMode:
		switch(index) {
		case VsMenu_Melee:     return MenuColor_Red;
		case VsMenu_Controls:  return MenuColor_Blue;
		case VsMenu_Options:   return MenuColor_Yellow;
		case VsMenu_DebugMenu: return MenuColor_Purple;
		case VsMenu_Manual:    return MenuColor_Green;
		}
	case MenuType_DebugMode:
		return MenuColor_Purple;
	case MenuType_Manual:
		return MenuColor_Green;
	case MenuType_Rules:
	case MenuType_ExtraRules:
	case MenuType_StageMusic:
	case MenuType_MenuMusic:
	case MenuType_NameEntry:
		return MenuColor_Red;
	default:
		return MenuColor_Yellow;
	}
}

extern "C" void orig_VsMenu_Think(HSD_GObj *gobj);
extern "C" void hook_VsMenu_Think(HSD_GObj *gobj)
{
	const auto buttons = Menu_GetButtonsHelper(PORT_ALL);

	if (buttons & MenuButton_Confirm) {
		IsEnteringMenu = true;

		switch (MenuSelectedIndex) {
		case VsMenu_Controls:
			Menu_PlaySFX(MenuSFX_Activate);
			store_controls_menu_port();
			Menu_ExitToScene(Scene_Controls);
			break;
		case VsMenu_Options:
			Menu_PlaySFX(MenuSFX_Activate);
			Menu_MainMenuTransition(MenuType_Options, 0, MenuState_EnterTo);
			break;
		case VsMenu_DebugMenu:
			if (is_20XX()) {
				if (get_settings_lock()) {
					// Don't allow using the debug menu when tournament locked
					Menu_PlaySFX(MenuSFX_Error);
					break;
				}
				Menu_PlaySFX(MenuSFX_Activate);
				Menu_ExitToScene(Scene_DebugMenu);
			} else {
				Menu_PlaySFX(MenuSFX_Activate);
				Menu_CreateLanguageMenu(MenuState_EnterTo);
				GObj_Free(gobj);
			}
			break;
		case VsMenu_Manual:
			MenuInputCooldown = 5;
			Menu_PlaySFX(MenuSFX_Activate);
			Menu_EnterCustomRulesMenu();
			GObj_Free(gobj);
			break;
		default:
			orig_VsMenu_Think(gobj);
		}
	} else if (buttons & MenuButton_B) {
		Menu_PlaySFX(MenuSFX_Back);
		Menu_ExitToScene(Scene_TitleScreen);
	} else {
			orig_VsMenu_Think(gobj);
	}

}

extern "C" void orig_OptionsMenu_Think(HSD_GObj *gobj);
extern "C" void hook_OptionsMenu_Think(HSD_GObj *gobj)
{
	const auto buttons = Menu_GetButtonsHelper(PORT_ALL);

	if (!(buttons & MenuButton_Confirm) && (buttons & MenuButton_B)) {
		IsEnteringMenu = false;
		Menu_PlaySFX(MenuSFX_Back);
		Menu_MainMenuTransition(MenuType_VsMode, VsMenu_Options, MenuState_ExitTo);
	} else {
		orig_OptionsMenu_Think(gobj);
	}
}

extern "C" void orig_MainMenu_Enter(SceneMinorData *minor);
extern "C" void hook_MainMenu_Enter(SceneMinorData *minor)
{
	orig_MainMenu_Enter(minor);

	auto *data = (MainMenuEnterData*)minor->data.enter_data;

	switch (SceneMajorPrevious) {
	case Scene_TitleScreen:
		// Go straight to VS Mode menu from title screen
		data->menu_type = MenuType_VsMode;
		break;
	case Scene_Controls:
		// Hover over controls portal when returning from controls menu
		data->selected = VsMenu_Controls;
		break;
	case Scene_DebugMenu:
		// Go back to debug menu portal when returning from debug menu
		data->menu_type = MenuType_VsMode;
		data->selected = VsMenu_DebugMenu;
		break;
	}
}

extern "C" void orig_MainMenu_Init(void *menu);
extern "C" void hook_MainMenu_Init(void *menu)
{
	orig_MainMenu_Init(menu);

	// Replace portal textures
	const auto *names = MenMainCursor_Top.matanim_joint->child->child->next->matanim;
	unmanaged_texture_swap(controls_portal_tex_data, names->texanim->imagetbl[11]);
	unmanaged_texture_swap(debug_menu_tex_data,      names->texanim->imagetbl[13]);
	unmanaged_texture_swap(manual_tex_data,          names->texanim->imagetbl[14]);

	// Replace preview textures
	const auto *previews =
		MenMainConTop_Top.matanim_joint->child->child->next->child->next->child->matanim;
	unmanaged_texture_swap(controls_preview_tex_data, previews->texanim->imagetbl[7]);
	unmanaged_texture_swap(options_preview_tex_data,  previews->texanim->imagetbl[3]);
	unmanaged_texture_swap(manual_preview_tex_data,   previews->texanim->imagetbl[8]);

	// Replace menu panel header
	const auto *header = MenMainPanel_Top.matanim_joint->child->next->next->child->
		next->next->next->next->matanim;
	unmanaged_texture_swap(manual_header_tex_data, header->texanim->imagetbl[9]);
	unmanaged_texture_swap(debug_header_tex_data,  header->texanim->imagetbl[14]);

	// Use original debug menu art on 20XX
	if (is_20XX())
		return;

	// Debug menu preview
	const auto *tournament_preview =
		MenMainConTop_Top.matanim_joint->child->child->next->child->
		next->next->next->next->next->next->next->next->next->matanim;
	unmanaged_texture_swap(debug_menu_preview_tex_data,
	                       tournament_preview->texanim->imagetbl[21]);
}

extern "C" void orig_Menu_UpdateMainMenuPreview(HSD_GObj *gobj, u8 index_changed);
extern "C" void hook_Menu_UpdateMainMenuPreview(HSD_GObj *gobj, u8 index_changed)
{
	orig_Menu_UpdateMainMenuPreview(gobj, index_changed);

	const auto *data = gobj->get<MainMenuData>();

	// Hide controllers
	if (data->menu_type == MenuType_VsMode && MenuSelectedIndex == VsMenu_DebugMenu)
		HSD_JObjSetFlagsAll(data->jobj_tree[36], HIDDEN);
}

static void update_portal_description(MainMenuData *data, u32 menu_type, u32 index)
{
	switch (menu_type) {
	case MenuType_VsMode:
		if (index == VsMenu_DebugMenu && is_20XX()) {
			// Use the original 20XX description for the shifted debug menu
			const auto &table = MenuTypeDataTable[MenuType_VsMode];
			const auto id = table.descriptions[VsMenu_Tournament];
			Text_SetFromSIS(data->description_text, id);
			return;
		}

		if (index == VsMenu_Controls && get_controls() != controls_type::z_jump)
			data->description_text->data = controls_description.data();
		else if (index == VsMenu_Controls)
			data->description_text->data = controls_z_jump_description.data();
		else if (index == VsMenu_Options)
			data->description_text->data = options_menu_description.data();
		else if (index == VsMenu_DebugMenu)
			data->description_text->data = debug_menu_description.data();
		else if (index == VsMenu_Manual)
			data->description_text->data = manual_description.data();

		break;
	}
}

extern "C" HSD_GObj *orig_Menu_SetupMainMenu(u8 state);
extern "C" HSD_GObj *hook_Menu_SetupMainMenu(u8 state)
{
	// Replace Special Melee with Options
	const auto *names = MenMainCursor_Top.matanim_joint->child->child->next->matanim;
	names->texanim->imagetbl[12] = names->texanim->imagetbl[3];

	// Replace preview background color tracks
	const auto *preview_bg =
		MenMainConTop_Top.matanim_joint->child->child->next->child->matanim;
	auto *fobj_r = preview_bg->aobjdesc->fobjdesc;
	auto *fobj_g = fobj_r->next;
	auto *fobj_b = fobj_g->next;
	fobj_r->ad = preview_color_r.data();
	fobj_g->ad = preview_color_g.data();
	fobj_b->ad = preview_color_b.data();

	auto *gobj = orig_Menu_SetupMainMenu(state);
	auto *data = gobj->get<MainMenuData>();

	update_portal_description(data, data->menu_type, data->selected);

	return gobj;
}

extern "C" void orig_Menu_CreatePortalDescriptionText(MainMenuData *data, u32 menu_type, u32 index);
extern "C" void hook_Menu_CreatePortalDescriptionText(MainMenuData *data, u32 menu_type, u32 index)
{
	orig_Menu_CreatePortalDescriptionText(data, menu_type, index);
	update_portal_description(data, menu_type, index);
}
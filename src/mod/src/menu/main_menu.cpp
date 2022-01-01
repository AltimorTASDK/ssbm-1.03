#include "hsd/archive.h"
#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "hsd/mobj.h"
#include "hsd/tobj.h"
#include "melee/menu.h"
#include "melee/scene.h"
#include "melee/text.h"
#include "menu/controls.h"
#include "rules/values.h"
#include "util/compression.h"
#include "util/meta.h"
#include "util/patch_list.h"
#include "util/melee/text_builder.h"
#include <gctypes.h>

#include "resources/portals/vs_mode_preview.tex.h"
#include "resources/portals/controls.tex.h"
#include "resources/portals/controls_preview.tex.h"
#include "resources/portals/debug_menu.tex.h"
#include "resources/portals/debug_menu_preview.tex.h"
#include "resources/portals/manual.tex.h"
#include "resources/portals/manual_preview.tex.h"
#include "resources/screens/manual_header.tex.h"

enum VsMenuPortalID {
	VsMenu_TournamentMelee = 1,
	VsMenu_Controls = 1,
	VsMenu_DebugMenu = 2,
	VsMenu_CustomRules = 3,
	VsMenu_Manual = 3
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
		text_builder::fade_interval<0, 0>(),
		text_builder::fit(),
		text_builder::ascii<line1>(),
		text_builder::end_fit(),
		text_builder::br(),
		text_builder::fade_interval<0, 0>(),
		text_builder::fit(),
		text_builder::ascii<line2>(),
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
		text_builder::fade_interval<0, 0>(),
		text_builder::fit(),
		text_builder::ascii<line1>(),
		text_builder::end_fit(),
		text_builder::reset_scale(),
		text_builder::end_color());
}

constexpr auto debug_menu_description = make_description_text<
	"Toggle DEVELOP mode and",
	"other options.">();

constexpr auto controls_description = make_description_text<
	"Remap buttons and change",
	"other controller settings.">();

constexpr auto controls_locked_description = make_description_text<
	"Remap buttons.">();

constexpr auto manual_description = make_description_text<
	"Read the 1.03 memory card",
	"manual.">();

static const auto patches = patch_list {
	// Swap previews for debug menu and controls
	std::pair { &MenuTypeDataTable[MenuType_VsMode].preview_anims[1], HSD_AnimLoop {
		800, 849, 820
	} },
	std::pair { &MenuTypeDataTable[MenuType_VsMode].preview_anims[2], HSD_AnimLoop {
		750, 799, 770
	} },
};

extern "C" void orig_VsMenu_Think(HSD_GObj *gobj);
extern "C" void hook_VsMenu_Think(HSD_GObj *gobj)
{
	if (!(Menu_GetButtonsHelper(PORT_ALL) & MenuButton_Confirm))
		return orig_VsMenu_Think(gobj);

	switch (MenuSelectedIndex) {
	case VsMenu_Controls:
		Menu_PlaySFX(MenuSFX_Activate);
		store_controls_menu_port();
		Menu_ExitToScene(Scene_Controls);
		break;
	case VsMenu_DebugMenu:
		if (get_settings_lock()) {
			// Don't allow using the debug menu when tournament lock is on
			Menu_PlaySFX(MenuSFX_Error);
			break;
		}
		Menu_PlaySFX(MenuSFX_Activate);
		Menu_ExitToScene(Scene_DebugMenu);
		break;
	default:
		orig_VsMenu_Think(gobj);
	}
}

extern "C" void orig_MainMenu_Enter(SceneMinorData *minor);
extern "C" void hook_MainMenu_Enter(SceneMinorData *minor)
{
	orig_MainMenu_Enter(minor);

	auto *data = (MainMenuEnterData*)minor->data.enter_data;

	// Hover over controls portal when returning from controls menu
	if (SceneMajorPrevious == Scene_Controls)
		data->selected = VsMenu_Controls;

	// Go back to debug menu portal when returning from debug menu
	if (SceneMajorPrevious == Scene_DebugMenu) {
		data->menu_type = MenuType_VsMode;
		data->selected = VsMenu_DebugMenu;
	}
}

extern "C" void orig_MainMenu_Init(void *menu);
extern "C" void hook_MainMenu_Init(void *menu)
{
	orig_MainMenu_Init(menu);

	// Replace portal textures
	const auto *names = MenMainCursor_Top.matanim_joint->child->child->next->matanim;
	unmanaged_texture_swap(controls_tex_data,   names->texanim->imagetbl[11]);
	unmanaged_texture_swap(debug_menu_tex_data, names->texanim->imagetbl[12]);
	unmanaged_texture_swap(manual_tex_data,     names->texanim->imagetbl[13]);

	// Replace preview textures
	const auto *previews =
		MenMainConTop_Top.matanim_joint->child->child->next->child->next->child->matanim;
	unmanaged_texture_swap(vs_mode_preview_tex_data,  previews->texanim->imagetbl[1]);
	unmanaged_texture_swap(controls_preview_tex_data, previews->texanim->imagetbl[7]);
	unmanaged_texture_swap(manual_preview_tex_data,   previews->texanim->imagetbl[8]);

	// Replace menu panel header
	const auto *header = MenMainPanel_Top.matanim_joint->child->next->next->child->
		next->next->next->next->matanim;
	unmanaged_texture_swap(manual_header_tex_data, header->texanim->imagetbl[8]);

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

	if (data->menu_type != MenuType_VsMode || MenuSelectedIndex != VsMenu_DebugMenu)
		return;

	// Hide controllers
	HSD_JObjSetFlagsAll(data->jobj_tree[36], HIDDEN);
}

static void update_portal_description(MainMenuData *data, u32 menu_type, u32 index)
{
	if (menu_type != MenuType_VsMode)
		return;

	if (index == VsMenu_DebugMenu)
		data->description_text->data = debug_menu_description.data();
	else if (index == VsMenu_Controls && !get_settings_lock())
		data->description_text->data = controls_description.data();
	else if (index == VsMenu_Controls && get_settings_lock())
		data->description_text->data = controls_locked_description.data();
	else if (index == VsMenu_Manual)
		data->description_text->data = manual_description.data();
}

extern "C" HSD_GObj *orig_Menu_SetupMainMenu(u8 state);
extern "C" HSD_GObj *hook_Menu_SetupMainMenu(u8 state)
{
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
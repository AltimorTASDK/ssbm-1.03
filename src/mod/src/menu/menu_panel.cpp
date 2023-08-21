#include "compat/unclepunch.h"
#include "hsd/dobj.h"
#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "hsd/mobj.h"
#include "hsd/tobj.h"
#include "melee/menu.h"
#include <gctypes.h>

enum MenuPanelState {
	MenuPanelState_Enter = 0,
	MenuPanelState_Exit  = 1,
	MenuPanelState_Idle  = 2
};

enum HeaderTex {
	HeaderTex_1PMode   = 0,
	HeaderTex_VsMode   = 1,
	HeaderTex_Trophies = 2,
	HeaderTex_Options  = 3,
	HeaderTex_Data     = 4,
};

struct MenuPanelData {
	u8 menu_type;
	u8 menu_type_previous;
	char pad002;
	u8 state;
};

struct MenuPanelAnim {
	HSD_AnimLoop enter;
	HSD_AnimLoop exit;
	HSD_AnimLoop idle;
};

extern "C" MenuPanelAnim MenuPanelAnimTable[MenuType_Max];

extern "C" void orig_Menu_MenuPanelThink(HSD_GObj *gobj);

[[gnu::constructor]] static void make_vs_mode_main_menu()
{
	MenuPanelAnimTable[MenuType_VsMode] = MenuPanelAnimTable[MenuType_Main];
}

static bool is_in_menu_music(const MenuPanelData *data)
{
	if (is_unclepunch_osd())
		return false;

	if (MenuType == MenuType_MenuMusic)
		return true;

	if (MenuTypePrevious != MenuType_MenuMusic)
		return false;

	// Check for exiting menu music, including first frame where state hasn't updated
	return data->menu_type == MenuType_ItemSwitch || data->state == MenuPanelState_Exit;
}

static bool handle_menu_music(HSD_GObj *gobj)
{
	const auto *data = gobj->get<MenuPanelData>();

	if (!is_in_menu_music(data))
		return false;

	bool first_frame;
	if (data->menu_type != MenuType_ItemSwitch)
		first_frame = MenuType == MenuType_MenuMusic; // First enter frame
	else if (MenuType != MenuType_MenuMusic)
		first_frame = data->state != MenuPanelState_Exit; // First exit frame
	else
		first_frame = false;

	auto *jobj = gobj->get_hsd_obj<HSD_JObj>();

	// Grid background for random stage/menu music
	auto *grid = HSD_JObjGetFromTreeByIndex(jobj, 98);

	// Back up grid anim so it doesn't get overwritten by item switch anim
	const auto grid_frame = HSD_JObjGetAnimFrame(grid);

	if (MenuType == MenuType_MenuMusic) {
		// Use item switch anim for header
		MenuType = MenuType_ItemSwitch;
		orig_Menu_MenuPanelThink(gobj);
		MenuType = MenuType_MenuMusic;
	} else {
		// Using item switch exit from data->menu_type_previous
		orig_Menu_MenuPanelThink(gobj);

		// Don't interfere with returning to rules menu idle anim
		if (data->state == MenuPanelState_Idle)
			return true;
	}

	const auto &ss_anim = MenuPanelAnimTable[MenuType_MenuMusic];

	// Hide item switch grid
	auto *is_grid = HSD_JObjGetFromTreeByIndex(jobj, 90);
	HSD_JObjSetFlagsAll(is_grid, HIDDEN);

	// Use random stage texture over item switch name
	auto *name1 = HSD_JObjGetFromTreeByIndex(jobj, 82);
	auto *name2 = HSD_JObjGetFromTreeByIndex(jobj, 83);
	name1->u.dobj->mobj->tobj->imagedesc = name2->u.dobj->mobj->tobj->imagedesc;

	if (first_frame) {
		// Start grid anim
		if (data->state == MenuPanelState_Enter)
			HSD_JObjReqAnimAll(grid, ss_anim.enter.start);
		else
			HSD_JObjReqAnimAll(grid, ss_anim.exit.start);
	} else {
		// Continue grid anim
		HSD_JObjReqAnimAll(grid, grid_frame);

		if (data->state == MenuPanelState_Enter)
			HSD_JObjLoopAnim(grid, ss_anim.enter);
		else if (data->state == MenuPanelState_Exit)
			HSD_JObjLoopAnim(grid, ss_anim.exit);
		else
			HSD_JObjLoopAnim(grid, ss_anim.idle);

	}

	HSD_JObjAnimAll(grid);

	return true;
}

static bool handle_debug_mode(HSD_GObj *gobj)
{
	if (MenuType != MenuType_DebugMode && MenuTypePrevious != MenuType_DebugMode)
		return false;

	// Replace the Options header with Main Menu in the Debug Mode menu
	auto *jobj = gobj->get_hsd_obj<HSD_JObj>();
	auto *header = HSD_JObjGetFromTreeByIndex(jobj, 84);
	auto *main_menu = HSD_JObjGetFromTreeByIndex(jobj, 86);
	auto *main_menu_tex = main_menu->u.dobj->mobj->tobj->imagedesc;
	auto *header_imagetbl = header->u.dobj->mobj->tobj->imagetbl;
	auto *old_options_tex = header_imagetbl[HeaderTex_Options];
	header_imagetbl[HeaderTex_Options] = main_menu_tex;
	orig_Menu_MenuPanelThink(gobj);
	header_imagetbl[HeaderTex_Options] = old_options_tex;

	return true;
}

static bool handle_manual(HSD_GObj *gobj)
{
	if (MenuType != MenuType_Manual && MenuTypePrevious != MenuType_Manual)
		return false;

	// Replace the Vs Mode header with Main Menu in the 1.03 Manual menu
	auto *jobj = gobj->get_hsd_obj<HSD_JObj>();
	auto *header = HSD_JObjGetFromTreeByIndex(jobj, 84);
	auto *main_menu = HSD_JObjGetFromTreeByIndex(jobj, 86);
	auto *main_menu_tex = main_menu->u.dobj->mobj->tobj->imagedesc;
	auto *header_imagetbl = header->u.dobj->mobj->tobj->imagetbl;
	auto *old_vs_mode_tex = header_imagetbl[HeaderTex_VsMode];
	header_imagetbl[HeaderTex_VsMode] = main_menu_tex;
	orig_Menu_MenuPanelThink(gobj);
	header_imagetbl[HeaderTex_VsMode] = old_vs_mode_tex;

	return true;
}

extern "C" void hook_Menu_MenuPanelThink(HSD_GObj *gobj)
{
	if (handle_menu_music(gobj))
		return;

	if (handle_debug_mode(gobj))
		return;

	if (handle_manual(gobj))
		return;

	orig_Menu_MenuPanelThink(gobj);
}
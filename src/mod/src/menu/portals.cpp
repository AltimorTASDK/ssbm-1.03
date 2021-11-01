#include "hsd/archive.h"
#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "hsd/mobj.h"
#include "hsd/tobj.h"
#include "melee/menu.h"
#include "melee/text.h"
#include "util/compression.h"
#include "util/melee/text_builder.h"
#include <gctypes.h>

#include "resources/portals/controls.tex.h"
#include "resources/portals/controls_preview.tex.h"
#include "resources/portals/debug_menu.tex.h"
#include "resources/portals/manual.tex.h"

enum VsMenuPortalID {
	VsMenu_TournamentMelee = 1,
	VsMenu_DebugMenu = 1,
	VsMenu_SpecialMelee = 2,
	VsMenu_Controls = 2,
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

extern "C" ArchiveModel MenMainCursor_Top;
extern "C" ArchiveModel MenMainConTop_Top;

constexpr auto debug_menu_description = text_builder::build(
	text_builder::kern(),
	text_builder::left(),
	text_builder::color<170, 170, 170>(),
	text_builder::textbox<179, 128>(),
	text_builder::offset<0, -20>(),
	text_builder::br(),
	text_builder::unk06<0, 0>(),
	text_builder::fit(),
	text_builder::ascii<"Toggle DEVELOP mode and">(),
	text_builder::end_fit(),
	text_builder::br(),
	text_builder::unk06<0, 0>(),
	text_builder::fit(),
	text_builder::ascii<"other options.">(),
	text_builder::end_fit(),
	text_builder::end_textbox(),
	text_builder::end_color());

constexpr auto controls_description = text_builder::build(
	text_builder::kern(),
	text_builder::left(),
	text_builder::color<170, 170, 170>(),
	text_builder::textbox<179, 128>(),
	text_builder::offset<0, -20>(),
	text_builder::br(),
	text_builder::unk06<0, 0>(),
	text_builder::fit(),
	text_builder::ascii<"Remap buttons and change">(),
	text_builder::end_fit(),
	text_builder::br(),
	text_builder::unk06<0, 0>(),
	text_builder::fit(),
	text_builder::ascii<"other controller settings.">(),
	text_builder::end_fit(),
	text_builder::end_textbox(),
	text_builder::end_color());

constexpr auto manual_description = text_builder::build(
	text_builder::kern(),
	text_builder::left(),
	text_builder::color<170, 170, 170>(),
	text_builder::textbox<179, 128>(),
	text_builder::offset<0, -20>(),
	text_builder::br(),
	text_builder::unk06<0, 0>(),
	text_builder::fit(),
	text_builder::ascii<"Read the 1.03 memory card">(),
	text_builder::end_fit(),
	text_builder::br(),
	text_builder::unk06<0, 0>(),
	text_builder::fit(),
	text_builder::ascii<"manual.">(),
	text_builder::end_fit(),
	text_builder::end_textbox(),
	text_builder::end_color());
	
extern "C" void orig_MainMenu_Init(void *menu);
extern "C" void hook_MainMenu_Init(void *menu)
{
	orig_MainMenu_Init(menu);
	
	// Replace portal textures
	const auto *names = MenMainCursor_Top.matanim_joint->child->child->next->matanim;
	names->texanim->imagetbl[11]->img_ptr = decompress(debug_menu_tex_data);
	names->texanim->imagetbl[12]->img_ptr = decompress(controls_tex_data);
	names->texanim->imagetbl[13]->img_ptr = decompress(manual_tex_data);

	// Replace preview textures
	const auto *previews =
		MenMainConTop_Top.matanim_joint->child->child->next->child->next->child->matanim;
	previews->texanim->imagetbl[7]->height = 132;
	previews->texanim->imagetbl[7]->img_ptr = decompress(controls_preview_tex_data);
}

static void update_portal_description(MainMenuData *data, u32 menu_type, u32 index)
{
	if (menu_type != MenuType_VsMode)
		return;
	
	if (index == VsMenu_DebugMenu)
		data->description_text->data = debug_menu_description.data();
	else if (index == VsMenu_Controls)
		data->description_text->data = controls_description.data();
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
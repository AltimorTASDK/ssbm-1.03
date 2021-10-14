#include "hsd/archive.h"
#include "hsd/mobj.h"
#include "hsd/tobj.h"
#include "melee/text.h"
#include "resources/portals/controls.tex.h"
#include "resources/portals/controls_preview.tex.h"
#include "resources/portals/debug_menu.tex.h"
#include "resources/portals/manual.tex.h"
#include "util/compression.h"
#include "util/melee/text_builder.h"
#include <gctypes.h>

enum MenuID {
	Menu_VsMenu = 2
};

enum VsMenuPortalID {
	VsMenu_TournamentMelee = 1,
	VsMenu_DebugMenu = 1,
	VsMenu_SpecialMelee = 2,
	VsMenu_Controls = 2,
	VsMenu_CustomRules = 3,
	VsMenu_Manual = 3
};

struct MainMenuData {
	char pad000[0xAC];
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

extern "C" void orig_Menu_SetupMainMenu(void *menu);
extern "C" void hook_Menu_SetupMainMenu(void *menu)
{
	orig_Menu_SetupMainMenu(menu);
	
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

extern "C" void orig_Menu_CreatePortalDescriptionText(MainMenuData *menu_data,
                                                      u32 menu_id, u32 portal_id);
extern "C" void hook_Menu_CreatePortalDescriptionText(MainMenuData *menu_data,
                                                      u32 menu_id, u32 portal_id)
{
	orig_Menu_CreatePortalDescriptionText(menu_data, menu_id, portal_id);

	if (menu_id != Menu_VsMenu)
		return;
	
	if (portal_id == VsMenu_DebugMenu)
		menu_data->description_text->data = debug_menu_description.data();
	else if (portal_id == VsMenu_Controls)
		menu_data->description_text->data = controls_description.data();
	else if (portal_id == VsMenu_Manual)
		menu_data->description_text->data = manual_description.data();
}
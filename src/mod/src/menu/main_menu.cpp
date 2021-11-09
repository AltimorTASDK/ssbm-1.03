#include "hsd/archive.h"
#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "hsd/mobj.h"
#include "hsd/tobj.h"
#include "melee/menu.h"
#include "melee/scene.h"
#include "melee/text.h"
#include "util/compression.h"
#include "util/meta.h"
#include "util/patch_list.h"
#include "util/melee/text_builder.h"
#include <gctypes.h>

#include "resources/portals/vs_mode_preview.tex.h"
#include "resources/portals/controls.tex.h"
#include "resources/portals/controls_preview.tex.h"
#include "resources/portals/debug_menu.tex.h"
#include "resources/portals/debug_menu_preview_20XX.tex.h"
#include "resources/portals/manual.tex.h"

enum VsMenuPortalID {
	VsMenu_TournamentMelee = 1,
	VsMenu_Controls = 1,
	VsMenu_SpecialMelee = 2,
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

struct MenuTypeData {
	HSD_AnimLoop *preview_anims;
	f32 anim_frame;
	void *pad008;
	u8 option_count;
	void(*think)();
};

extern "C" ArchiveModel MenMainCursor_Top;
extern "C" ArchiveModel MenMainConTop_Top;
extern "C" MenuTypeData MenuTypeDataTable[MenuType_Max];

extern "C" void VsMenu_Think();

template<string_literal line1, string_literal line2>
static constexpr auto make_description_text()
{
	return text_builder::build(
		text_builder::kern(),
		text_builder::left(),
		text_builder::color<170, 170, 170>(),
		text_builder::textbox<179, 128>(),
		text_builder::offset<0, -20>(),
		text_builder::br(),
		text_builder::unk06<0, 0>(),
		text_builder::fit(),
		text_builder::ascii<line1>(),
		text_builder::end_fit(),
		text_builder::br(),
		text_builder::unk06<0, 0>(),
		text_builder::fit(),
		text_builder::ascii<line2>(),
		text_builder::end_fit(),
		text_builder::end_textbox(),
		text_builder::end_color());
}

constexpr auto debug_menu_description = make_description_text<
	"Toggle DEVELOP mode and",
	"other options.">();

constexpr auto controls_description = make_description_text<
	"Remap buttons and change",
	"other controller settings.">();

constexpr auto manual_description = make_description_text<
	"Read the 1.03 memory card",
	"other controller settings.">();

static const auto patches = patch_list {
	// Swap previews for debug menu and controls
	std::pair { &MenuTypeDataTable[MenuType_VsMode].preview_anims[1], HSD_AnimLoop {
		800, 849, 820
	} },
	std::pair { &MenuTypeDataTable[MenuType_VsMode].preview_anims[2], HSD_AnimLoop {
		750, 799, 770
	} },
	// Swap branches for entering debug menu and controls
	// beq 0x44
	std::pair { (char*)VsMenu_Think+0x54, 0x41820044 },
	// bge 0x50
	std::pair { (char*)VsMenu_Think+0x64, 0x40800050 },
};

extern "C" void orig_MainMenu_Enter(SceneMinorData *minor);
extern "C" void hook_MainMenu_Enter(SceneMinorData *minor)
{
	orig_MainMenu_Enter(minor);

	auto *data = (MainMenuEnterData*)minor->data.enter_data;

	// Hover over controls portal when returning from controls menu
	if (SceneMajorPrevious == Scene_Controls)
		data->selected = 1;
	
	// Go back to debug menu portal when returning from debug menu
	if (SceneMajorPrevious == Scene_DebugMenu) {
		data->menu_type = MenuType_VsMode;
		data->selected = 2;
	}
}
	
extern "C" void orig_MainMenu_Init(void *menu);
extern "C" void hook_MainMenu_Init(void *menu)
{
	orig_MainMenu_Init(menu);
	
	// Replace portal textures
	const auto *names = MenMainCursor_Top.matanim_joint->child->child->next->matanim;
	names->texanim->imagetbl[11]->img_ptr = decompress(controls_tex_data);
	names->texanim->imagetbl[12]->img_ptr = decompress(debug_menu_tex_data);
	names->texanim->imagetbl[13]->img_ptr = decompress(manual_tex_data);

	// Replace preview textures
	const auto *previews =
		MenMainConTop_Top.matanim_joint->child->child->next->child->next->child->matanim;
	previews->texanim->imagetbl[1]->img_ptr = decompress(vs_mode_preview_tex_data);
	previews->texanim->imagetbl[7]->height = 132;
	previews->texanim->imagetbl[7]->img_ptr = decompress(controls_preview_tex_data);

#if 0
	const auto *tournament_preview =
		MenMainConTop_Top.matanim_joint->child->child->next->child->
		next->next->next->next->next->next->next->next->next->matanim;
	tournament_preview->texanim->imagetbl[21]->img_ptr =
		decompress(debug_menu_preview_20XX_tex_data);
#endif
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
#include "hsd/archive.h"
#include "hsd/dobj.h"
#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "hsd/memory.h"
#include "hsd/mobj.h"
#include "hsd/tobj.h"
#include "melee/menu.h"
#include "melee/rules.h"
#include "melee/scene.h"
#include "melee/text.h"
#include "menu/stage_select.h"
#include "rules/saved_config.h"
#include "rules/settings_lock.h"
#include "util/compression.h"
#include "util/mempool.h"
#include "util/meta.h"
#include "util/patch_list.h"
#include "util/melee/text_builder.h"
#include <gctypes.h>
#include <ogc/gx.h>

#include "resources/rules/pause_values.tex.h"
#include "resources/rules/controller_fix.tex.h"
#include "resources/rules/controller_fix_values.tex.h"
#include "resources/rules/latency.tex.h"
#include "resources/rules/latency_values.tex.h"
#include "resources/rules/widescreen.tex.h"
#include "resources/rules/og_stage_select.tex.h"

struct ExtraRulesMenuData {
	u8 menu_type;
	u8 selected;
	u8 stock_time_limit;
	// Pause is moved up a row
	u8 pause;
	union {
		u8 friendly_fire;
		ucf_type controller_fix;
	};
	union {
		u8 score_display;
		latency_mode latency;
	};
	union {
		u8 sd_penalty;
		u8 widescreen;
	};
	u8 pad007;
	u8 state;
	struct {
		HSD_JObj *root1;
		HSD_JObj *root2;
		HSD_JObj *root3;
		HSD_JObj *rules[Rule_Max];
	} jobj_tree;
	struct {
		HSD_JObj *tree[7];
	} value_jobj_trees[ExtraRule_Max];
	Text *description_text;
};

// Rule name text
extern "C" ArchiveModel MenMainCursorRl_Top;

// Vanilla handicap values
extern "C" ArchiveModel MenMainCursorRl03_Top;

// Controller fix values
extern "C" ArchiveModel MenMainCursorTr03_Top;

// "Ready to start" banner animation frame
extern "C" u8 CSSReadyFrames;

extern "C" HSD_GObj *ExtraRulesMenuGObj;

extern "C" struct {
	f32 unselected;
	f32 selected;
} ExtraRuleTextAnimFrames[6];

extern "C" struct {
	u8 values[3];
} ExtraRuleDescriptions[6];

extern "C" HSD_GObj *Menu_SetupExtraRulesMenu(u8 state);

template<string_literal line1, string_literal line2>
static constexpr auto make_description_text()
{
	return text_builder::build(
		text_builder::kern(),
		text_builder::left(),
		text_builder::color<170, 170, 170>(),
		text_builder::textbox<179, 179>(),
		text_builder::unk06<0, 0>(),
		text_builder::fit(),
		text_builder::ascii<line1>(),
		text_builder::end_fit(),
		text_builder::br(),
		text_builder::fit(),
		text_builder::ascii<line2>(),
		text_builder::end_fit(),
		text_builder::end_textbox(),
		text_builder::end_color());
}

constexpr auto ucf_type_descriptions = multi_array {
	make_description_text<"Maximize shield drop's range and fix",
		              "1.0 cardinal and dash out of crouch.">(),
	make_description_text<"Reduce shield drop's range and remove 1.0",
		              "cardinal and dash out of crouch fixes.">()
};

constexpr auto latency_descriptions = multi_array {
	make_description_text<"Play with the normal amount",
		              "of latency.">(),
	make_description_text<"Reduce latency by half a frame.",
		              "Recommended for LCDs.">(),
	make_description_text<"Reduce latency by one and a half",
		              "frames. Recommended only for Wii.">()
};

constexpr auto widescreen_descriptions = multi_array {
	make_description_text<"Play with the normal aspect",
		              "ratio.">(),
	make_description_text<"Play with a widescreen aspect",
		              "ratio.">()
};

constexpr auto oss_description = text_builder::build(
	text_builder::kern(),
	text_builder::left(),
	text_builder::color<170, 170, 170>(),
	text_builder::textbox<179, 179>(),
	text_builder::unk06<0, 0>(),
	text_builder::fit(),
	text_builder::ascii<"Open the original stage select screen">(),
	text_builder::end_fit(),
	text_builder::br(),
	text_builder::fit(),
	text_builder::ascii<"and play without stage modifications.">(),
	text_builder::end_fit(),
	text_builder::end_textbox(),
	text_builder::end_color());

constexpr auto pause_auto_description = text_builder::build(
	text_builder::kern(),
	text_builder::left(),
	text_builder::color<170, 170, 170>(),
	text_builder::textbox<179, 179>(),
	text_builder::unk06<0, 0>(),
	text_builder::fit(),
	text_builder::ascii<"Players will not be able to">(),
	text_builder::end_fit(),
	text_builder::br(),
	text_builder::fit(),
	text_builder::ascii<"pause during 4-stock matches.">(),
	text_builder::end_fit(),
	text_builder::end_textbox(),
	text_builder::end_color());

static mempool pool;
static texture_swap *decompressed_textures[ExtraRule_Max];

static const auto patches = patch_list {
	// Swap text for pause and friendly fire
	std::pair { &ExtraRuleTextAnimFrames[ExtraRule_Pause].unselected,        24.f },
	std::pair { &ExtraRuleTextAnimFrames[ExtraRule_Pause].selected,          25.f },
	std::pair { &ExtraRuleTextAnimFrames[ExtraRule_FriendlyFire].unselected, 22.f },
	std::pair { &ExtraRuleTextAnimFrames[ExtraRule_FriendlyFire].selected,   23.f },

	// Swap description for pause and friendly fire
	std::pair { &ExtraRuleDescriptions[ExtraRule_Pause].values[1],           (u8)0x3B },

	// Apply 3-value model to index 3 instead of 4
	// cmpwi r27, 3
	std::pair { (char*)Menu_SetupExtraRulesMenu+0x574,                       0x2C1B0003u },
	// nop
	std::pair { (char*)Menu_SetupExtraRulesMenu+0x57C,                       0x60000000u },
};

static void replace_toggle_texture(ExtraRulesMenuData *data, int index)
{
	auto *tobj = data->value_jobj_trees[index].tree[1]->u.dobj->mobj->tobj;
	tobj->imagedesc = decompressed_textures[index]->image;
}

static void pool_free(void *data)
{
	HSD_Free(data); // Default free gobj data
	pool.dec_ref();
}

static void load_textures()
{
	// Replace rule name textures
	const auto *rule_names = MenMainCursorRl_Top.matanim_joint->child->child->next->matanim;
	pool.add(new texture_swap(controller_fix_tex_data,  rule_names->texanim->imagetbl[ 9]));
	pool.add(new texture_swap(latency_tex_data,         rule_names->texanim->imagetbl[11]));
	pool.add(new texture_swap(widescreen_tex_data,      rule_names->texanim->imagetbl[12]));
	pool.add(new texture_swap(og_stage_select_tex_data, rule_names->texanim->imagetbl[13]));

	// Load rule value textures
	decompressed_textures[ExtraRule_Pause] =
		pool.add(new texture_swap(pause_values_tex_data));

	decompressed_textures[ExtraRule_ControllerFix] =
		pool.add(new texture_swap(controller_fix_values_tex_data));

	decompressed_textures[ExtraRule_Latency] =
		pool.add(new texture_swap(latency_values_tex_data));
}

extern "C" HSD_GObj *orig_Menu_SetupExtraRulesMenu(u8 state);
extern "C" HSD_GObj *hook_Menu_SetupExtraRulesMenu(u8 state)
{
	auto *gobj = orig_Menu_SetupExtraRulesMenu(state);
	auto *data = gobj->get<ExtraRulesMenuData>();

	// Free assets on menu exit
	gobj->user_data_remove_func = pool_free;

	if (pool.inc_ref() == 0)
		load_textures();

	// Replace rule value textures
	replace_toggle_texture(data, ExtraRule_Pause);
	replace_toggle_texture(data, ExtraRule_ControllerFix);
	replace_toggle_texture(data, ExtraRule_Latency);

	return gobj;
}

static void save_config()
{
	const auto *data = ExtraRulesMenuGObj->get<ExtraRulesMenuData>();
	config.widescreen = data->widescreen;
	config.latency = data->latency;
	config.save();
}

extern "C" void orig_Menu_ExtraRulesMenuInput(HSD_GObj *gobj);
extern "C" void hook_Menu_ExtraRulesMenuInput(HSD_GObj *gobj)
{
	const auto buttons = Menu_GetButtonsHelper(PORT_ALL);

	if (buttons & (MenuButton_B | MenuButton_Start))
		save_config();

	if ((buttons & MenuButton_A) && MenuSelectedIndex == ExtraRule_OldStageSelect) {
		// Check for attempting to enter the OSS with 0 players
		if (CSSReadyFrames == 0) {
			Menu_PlaySFX(MenuSFX_Denied);
			return;
		}

		save_config();
	}

	if (settings_locked && MenuSelectedIndex < ExtraRule_OldStageSelect) {
		if (buttons & (MenuButton_Left | MenuButton_Right)) {
			// Don't allow changing rules with settings locked
			Menu_PlaySFX(MenuSFX_Denied);
			return;
		}
	}

	orig_Menu_ExtraRulesMenuInput(gobj);
}

extern "C" void orig_Menu_CreateRandomStageMenu();
extern "C" void hook_Menu_CreateRandomStageMenu()
{
	if (MenuType != MenuType_ExtraRules)
		return orig_Menu_CreateRandomStageMenu();

	// Go to stage select if coming from extra rules
	use_og_stage_select = true;
	Menu_ExitToMinorScene(VsScene_SSS);
}

extern "C" void orig_Menu_UpdateExtraRuleDescriptionText(HSD_GObj *gobj,
                                                         bool index_changed, bool value_changed);
extern "C" void hook_Menu_UpdateExtraRuleDescriptionText(HSD_GObj *gobj,
                                                         bool index_changed, bool value_changed)
{
	orig_Menu_UpdateExtraRuleDescriptionText(gobj, index_changed, value_changed);

	if (!index_changed && !value_changed)
		return;

	auto *data = gobj->get<ExtraRulesMenuData>();
	auto *text = data->description_text;

	const auto index = MenuSelectedIndex;
	const auto value = MenuSelectedValue;

	if (index == ExtraRule_Pause && !value) {
		text->data = pause_auto_description.data();
		return;
	}

	switch (index) {
	case ExtraRule_ControllerFix:  text->data = ucf_type_descriptions[value]; break;
	case ExtraRule_Latency:        text->data = latency_descriptions[value]; break;
	case ExtraRule_Widescreen:     text->data = widescreen_descriptions[value]; break;
	case ExtraRule_OldStageSelect: text->data = oss_description.data(); break;
	}
}
#include "hsd/archive.h"
#include "hsd/dobj.h"
#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "hsd/memory.h"
#include "hsd/mobj.h"
#include "hsd/tobj.h"
#include "latency/pdf_vb.h"
#include "melee/menu.h"
#include "melee/preferences.h"
#include "melee/rules.h"
#include "melee/scene.h"
#include "melee/text.h"
#include "rules/saved_config.h"
#include "rules/values.h"
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
#include "resources/rules/stage_mods.tex.h"
#include "resources/rules/stage_mods_values.tex.h"

struct ExtraRulesMenuData {
	u8 menu_type;
	u8 selected;
	union {
		struct {
			u8 stock_time_limit;
			// Pause is moved up a row
			u8 pause;
			stage_mod_type stage_mods;
			controls_type controls;
			ucf_type controller_fix;
			latency_mode latency;
			u8 widescreen;
		};
		u8 values[ExtraRule_Max];
	};
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

// Mode values model
extern "C" ArchiveModel MenMainCursorRl01_Top;

// Extra rule values models
extern "C" ArchiveModel MenMainCursorTr01_Top;
extern "C" ArchiveModel MenMainCursorTr02_Top;
extern "C" ArchiveModel MenMainCursorTr03_Top;
extern "C" ArchiveModel MenMainCursorTr04_Top;

extern "C" HSD_GObj *ExtraRulesMenuGObj;

extern "C" u8 FeatureUnlockMask;

extern "C" struct {
	f32 unselected;
	f32 selected;
} ExtraRuleTextAnimFrames[6];

extern "C" HSD_AnimLoop RuleValueAnimLoops[10];

extern "C" HSD_GObj *Menu_SetupExtraRulesMenu(u8 state);
extern "C" void Menu_ExtraRulesMenuInput(HSD_GObj *gobj);
extern "C" void Menu_ExtraRulesMenuThink(HSD_GObj *gobj);
extern "C" void Menu_UpdateExtraRuleDisplay(HSD_GObj *gobj, bool index_changed, bool value_changed);
extern "C" const HSD_AnimLoop &Menu_GetExtraRuleValueAnimLoop(u8 index, u8 value, bool scroll_left);
extern "C" void Menu_UpdateExtraRuleValueAnim(HSD_GObj *gobj, HSD_JObj *jobj, u8 index);

template<string_literal line1, string_literal line2>
static constexpr auto make_description_text()
{
	return text_builder::build(
		text_builder::kern(),
		text_builder::left(),
		text_builder::color<170, 170, 170>(),
		text_builder::scale<179, 179>(),
		text_builder::fade_interval<0, 0>(),
		text_builder::fit(),
		text_builder::ascii<line1>(),
		text_builder::end_fit(),
		text_builder::br(),
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
		text_builder::offset<0, -10>(),
		text_builder::br(),
		text_builder::fade_interval<0, 0>(),
		text_builder::fit(),
		text_builder::ascii<line1>(),
		text_builder::end_fit(),
		text_builder::reset_scale(),
		text_builder::end_color());
}

constexpr auto pause_auto_description =
	make_description_text<"Players will not be able to",
	                      "pause during 4-stock matches.">();

constexpr auto stage_mod_descriptions = multi_array {
	make_description_text<"Modify all tournament legal",
	                      "stages except for Battlefield.">(),
	make_description_text<"Use the original stage select screen",
	                      "and play without stage modifications.">(),
	make_description_text<"Play without stage modifications.">(),
	make_description_text<u"Freeze Final Destination and",
	                      u"Pokémon Stadium.">()
};

constexpr auto ucf_type_descriptions = multi_array {
	make_description_text<"Improve shield drop and wiggle and fix",
		              "1.0 cardinal and dash out of crouch.">(),
	make_description_text<"Lessen shield drop and wiggle and remove",
		              "1.0 cardinal and dash out of crouch fixes.">()
};

constexpr auto latency_descriptions = multi_array {
	make_description_text<"Play with the normal amount",
		              "of latency.">(),
	make_description_text<"Reduce latency by half a frame.",
		              "Recommended for LCDs.">(),
	make_description_text<"Reduce latency by one and a half",
		              "frames. For use on Wii only.">()
};

constexpr auto widescreen_descriptions = multi_array {
	make_description_text<"Play with the normal aspect",
		              "ratio.">(),
	make_description_text<"Play with a widescreen aspect",
		              "ratio.">()
};

static mempool pool;
static texture_swap *decompressed_textures[ExtraRule_Max];

static const auto patches = patch_list {
	// Swap text for pause and friendly fire
	std::pair { &ExtraRuleTextAnimFrames[ExtraRule_Pause].unselected,        24.f },
	std::pair { &ExtraRuleTextAnimFrames[ExtraRule_Pause].selected,          25.f },
	std::pair { &ExtraRuleTextAnimFrames[ExtraRule_FriendlyFire].unselected, 22.f },
	std::pair { &ExtraRuleTextAnimFrames[ExtraRule_FriendlyFire].selected,   23.f },
	// Add a value model for index 5
	// nop
	std::pair { (char*)Menu_SetupExtraRulesMenu+0x558,                       0x60000000u },
	// Allow scrolling left/right on index 5
	// nop
	std::pair { (char*)Menu_ExtraRulesMenuInput+0x394,                       0x60000000u },
	// Don't allow pressing A on index 5
	// b 0x3CC
	std::pair { (char*)Menu_ExtraRulesMenuInput+0x50,                        0x480003CCu },
	// Skip index 5 checks
	// b 0x14
	std::pair { (char*)Menu_UpdateExtraRuleDisplay+0x218,                    0x48000014u },
	// nop
	std::pair { (char*)Menu_UpdateExtraRuleDisplay+0x2B8,                    0x60000000u },
	// b 0x28
	std::pair { (char*)Menu_UpdateExtraRuleDisplay+0x330,                    0x48000028u },
	// nop
	std::pair { (char*)Menu_UpdateExtraRuleDisplay+0x388,                    0x60000000u },
	std::pair { (char*)Menu_UpdateExtraRuleDisplay+0x38C,                    0x60000000u },
	// nop
	std::pair { (char*)Menu_UpdateExtraRuleDisplay+0x4FC,                    0x60000000u },
	// b 0x20
	std::pair { (char*)Menu_UpdateExtraRuleDisplay+0x510,                    0x48000020u },
	// b 0x24
	std::pair { (char*)Menu_GetExtraRuleValueAnimLoop+0x10,                  0x48000024u },
	// b 0x8
	std::pair { (char*)Menu_UpdateExtraRuleValueAnim+0x1C,                   0x48000008u },

	// Add a 7th rotator to extra rules
	std::pair { &MenuTypeDataTable[MenuType_ExtraRules].option_count,        (u8)7 },
	// cmpwi r31, 7
	std::pair { (char*)Menu_SetupExtraRulesMenu+0x388,                       0x2C1F0007u },
	// li r0, 6
	std::pair { (char*)Menu_ExtraRulesMenuInput+0x214,                       0x38000006u },
	// cmpwi r3, 6
	std::pair { (char*)Menu_ExtraRulesMenuInput+0x2D8,                       0x2C030006u },
	// cmplwi r5, 6
	std::pair { (char*)Menu_ExtraRulesMenuInput+0x390,                       0x28050006u },

	// Move data->state from 0x08 to 0x09
	// stb r16, 0x9(r25)
	std::pair { (char*)Menu_SetupExtraRulesMenu+0x220,                       0x9A190009u },
	// lbz r0, 0x9(r25)
	std::pair { (char*)Menu_SetupExtraRulesMenu+0x27C,                       0x88190008u },
	// lbz r0, 0x9(r31)
	std::pair { (char*)Menu_ExtraRulesMenuThink+0x2C,                        0x881F0009u },
	// stb r0, 0x9(r31)
	std::pair { (char*)Menu_ExtraRulesMenuThink+0x6C,                        0x981F0009u },
	// stb r0, 0x9(r31)
	std::pair { (char*)Menu_ExtraRulesMenuThink+0x78,                        0x981F0009u },
	// lbz r0, 0x9(r31)
	std::pair { (char*)Menu_ExtraRulesMenuThink+0x7C,                        0x881F0009u },
	// lbz r0, 0x9(r31)
	std::pair { (char*)Menu_ExtraRulesMenuThink+0xDC,                        0x881F0009u },
	// lbz r0, 0x9(r31)
	std::pair { (char*)Menu_ExtraRulesMenuThink+0x104,                       0x881F0009u },
	// lbz r0, 0x9(r31)
	std::pair { (char*)Menu_ExtraRulesMenuThink+0x170,                       0x881F0009u },
	// stb r0, 0x9(r31)
	std::pair { (char*)Menu_ExtraRulesMenuThink+0x1A0,                       0x981F0009u },
	// lbz r0, 0x9(r31)
	std::pair { (char*)Menu_ExtraRulesMenuThink+0x1BC,                       0x881F0009u },

	// Increase ExtraRulesMenuData size from 0xE0 to 0xE4
	// li r3, 0xE4
	std::pair { (char*)Menu_SetupExtraRulesMenu+0x180,                       0x386000E4u },

	// Move data->description_text from 0xDC to 0xE0
	// stw r24, 0xE0(r25)
	std::pair { (char*)Menu_SetupExtraRulesMenu+0x224,                       0x931900E0u },
	// lwz r3, 0xE0(r25)
	std::pair { (char*)Menu_SetupExtraRulesMenu+0x72C,                       0x807900E0u },
	// stw r0, 0xE0(r25)
	std::pair { (char*)Menu_SetupExtraRulesMenu+0x748,                       0x901900E0u },
	// stw r3, 0xE0(r25)
	std::pair { (char*)Menu_SetupExtraRulesMenu+0x79C,                       0x907900E0u },
};

static int get_jobj_index(int index)
{
	// Fix up indices if latency is hidden
	if (!is_faster_melee() || index < ExtraRule_Latency)
		return index;
	else
		return index - 1;
}

static void replace_toggle_texture(ExtraRulesMenuData *data, int index)
{
	auto *tobj = data->value_jobj_trees[index].tree[1]->u.dobj->mobj->tobj;
	tobj->imagedesc = decompressed_textures[index]->image;
}

static void set_to_rotator(ExtraRulesMenuData *data, int index)
{
	auto *cursor = data->jobj_tree.rules[get_jobj_index(index)]->child;
	auto [background, arrow, scroll] = HSD_JObjGetFromTreeTuple<6, 8, 13>(cursor);

	// Show black value background
	HSD_JObjClearFlagsAll(background, HIDDEN);

	// Initialize value jobj tree
	HSD_JObjGetTree<2>(cursor->next, data->value_jobj_trees[index].tree);

	if (data->selected == index) {
		// Show scroll arrows
		HSD_JObjClearFlagsAll(scroll, HIDDEN);
		// Hide arrow used for portals
		HSD_JObjSetFlagsAll(arrow, HIDDEN);
	}
}

static void fix_value_position(ExtraRulesMenuData *data, int index)
{
	// Set to proper position for additional rules value
	auto *jobj = data->value_jobj_trees[index].tree[1];
	jobj->position.x = 4.8f;
	HSD_JObjSetMtxDirty(jobj);
}

static void set_value_anim(ExtraRulesMenuData *data, int index)
{
	// Set initial value anim frame with support for more than 3 values
	auto *jobj = data->value_jobj_trees[index].tree[0];
	const auto value = data->values[index];

	const auto anim_index = value == 0 ? ExtraRuleValueBounds[index].max : value - 1;
	HSD_JObjReqAnimAll(jobj, RuleValueAnimLoops[anim_index].start);
	HSD_JObjAnimAll(jobj);
}

static void fix_rule_scale(ExtraRulesMenuData *data, int index)
{
	auto *cursor = data->jobj_tree.rules[get_jobj_index(index)]->child;
	auto *label = HSD_JObjGetFromTreeByIndex(cursor, 7);
	label->scale.x = 1.5f;
	HSD_JObjSetMtxDirty(label);
}

static void fix_rule_anims(ExtraRulesMenuData *data)
{
	// Force same label scale for all languages
	fix_rule_scale(data, ExtraRule_StageMods);
	fix_rule_scale(data, ExtraRule_ControllerFix);
	if (!is_faster_melee())
		fix_rule_scale(data, ExtraRule_Latency);
	fix_rule_scale(data, ExtraRule_Widescreen);
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
	pool.add_texture_swap(stage_mods_tex_data,      rule_names->texanim->imagetbl[ 9]);
	pool.add_texture_swap(controller_fix_tex_data,  rule_names->texanim->imagetbl[11]);
	pool.add_texture_swap(latency_tex_data,         rule_names->texanim->imagetbl[12]);
	pool.add_texture_swap(widescreen_tex_data,      rule_names->texanim->imagetbl[13]);

	// Load rule value textures
	decompressed_textures[ExtraRule_Pause] =
		pool.add_texture_swap(pause_values_tex_data);

	decompressed_textures[ExtraRule_StageMods] =
		pool.add_texture_swap(stage_mods_values_tex_data);

	decompressed_textures[ExtraRule_ControllerFix] =
		pool.add_texture_swap(controller_fix_values_tex_data);

	decompressed_textures[ExtraRule_Latency] =
		pool.add_texture_swap(latency_values_tex_data);
}

extern "C" ArchiveModel *select_extra_rule_model(u32 index)
{
	return std::array {
		&MenMainCursorTr01_Top, // Stock Match Time Limit
		&MenMainCursorTr03_Top, // Pause
		&MenMainCursorRl01_Top, // Stage Modifications
		&MenMainCursorTr04_Top, // Controls
		&MenMainCursorTr03_Top, // Controller Fix
		&MenMainCursorTr04_Top, // Latency
		&MenMainCursorTr03_Top, // Widescreen
	}[index];
}

extern "C" HSD_GObj *orig_Menu_SetupExtraRulesMenu(u8 state);
extern "C" HSD_GObj *hook_Menu_SetupExtraRulesMenu(u8 state)
{
	// Ensure all of the options always get populated
	FeatureUnlockMask = 0xFF;

	auto *gobj = orig_Menu_SetupExtraRulesMenu(state);
	auto *data = gobj->get<ExtraRulesMenuData>();

	data->widescreen = GetGameRules()->widescreen;

	// Free assets on menu exit
	gobj->user_data_remove_func = pool_free;

	if (pool.inc_ref() == 0)
		load_textures();

	// Replace rule value textures
	replace_toggle_texture(data, ExtraRule_Pause);
	replace_toggle_texture(data, ExtraRule_StageMods);
	replace_toggle_texture(data, ExtraRule_ControllerFix);
	if (!is_faster_melee())
		replace_toggle_texture(data, ExtraRule_Latency);

	// Make Rl01 use proper additional rules position
	fix_value_position(data, ExtraRule_StageMods);

	// Set anim frame correctly for 4-value model
	set_value_anim(data, ExtraRule_StageMods);

	// Use rotator for option replacing random stage select
	set_to_rotator(data, ExtraRule_RandomStage);
	set_value_anim(data, ExtraRule_RandomStage);

	fix_rule_anims(data);

	return gobj;
}

extern "C" bool orig_Menu_IsExtraRuleVisible(u8 index);
extern "C" bool hook_Menu_IsExtraRuleVisible(u8 index)
{
	return index != ExtraRule_Latency || !is_faster_melee();
}

extern "C" void orig_Menu_ExtraRulesMenuInput(HSD_GObj *gobj);
extern "C" void hook_Menu_ExtraRulesMenuInput(HSD_GObj *gobj)
{
	const auto buttons = Menu_GetButtonsHelper(PORT_ALL);

	if (get_settings_lock() && (buttons & (MenuButton_Left | MenuButton_Right))) {
		// Don't allow changing rules with settings locked
		Menu_PlaySFX(MenuSFX_Error);
		return;
	}

	orig_Menu_ExtraRulesMenuInput(gobj);

	if (!(buttons & MenuButton_A) && (buttons & (MenuButton_B | MenuButton_Start)))
		config.save();

	if (!is_faster_melee() || MenuSelectedIndex != ExtraRule_Latency)
		return;

	// Skip over the nonexistent latency rotator on FM
	if (buttons & MenuButton_Up)
		MenuSelectedIndex--;
	else
		MenuSelectedIndex++;

	const auto *data = ExtraRulesMenuGObj->get<ExtraRulesMenuData>();
	MenuSelectedValue = data->values[MenuSelectedIndex];
}

extern "C" const HSD_AnimLoop &orig_Menu_GetExtraRuleValueAnimLoop(u8 index, u8 value,
                                                                   bool scroll_right);
extern "C" const HSD_AnimLoop &hook_Menu_GetExtraRuleValueAnimLoop(u8 index, u8 value,
                                                                   bool scroll_right)
{
	if (index != ExtraRule_StageMods)
		return orig_Menu_GetExtraRuleValueAnimLoop(index, value, scroll_right);

	if (!scroll_right)
		return RuleValueAnimLoops[ExtraRuleValueBounds[index].max - value + 5];
	else if (value == 0)
		return RuleValueAnimLoops[ExtraRuleValueBounds[index].max];
	else
		return RuleValueAnimLoops[value - 1];
}

extern "C" void orig_Menu_UpdateExtraRuleValueAnim(HSD_GObj *gobj, HSD_JObj *jobj, u8 index);
extern "C" void hook_Menu_UpdateExtraRuleValueAnim(HSD_GObj *gobj, HSD_JObj *jobj, u8 index)
{
	if (index != ExtraRule_StageMods)
		return orig_Menu_UpdateExtraRuleValueAnim(gobj, jobj, index);

	const auto frame = HSD_JObjGetAnimFrame(jobj);

	for (u8 i = 0; i < 10; i++) {
		const auto &loop = RuleValueAnimLoops[i];
		if (frame >= loop.start && frame <= loop.end) {
			HSD_JObjLoopAnim(jobj, loop);
			return;
		}
	}
}

extern "C" void hook_Menu_UpdateExtraRuleDescriptionText(HSD_GObj *gobj,
                                                         bool index_changed, bool value_changed)
{
	auto *data = gobj->get<ExtraRulesMenuData>();
	auto *text = data->description_text;

	// Fix all rule anims since changes will have been overridden
	fix_rule_anims(data);

	if (data->state == MenuState_ExitFrom || data->state == MenuState_EnterFrom) {
		if (text != nullptr) {
			// Free text when leaving menu
			Text_Free(text);
			data->description_text = nullptr;
		}
		return;
	}

	if (text == nullptr) {
		// Create description text
		text = Text_Create(0, 1, -9.5f, 8.0f, 17.f, 364.68331909f, 76.77543640f);
		text->stretch = { 0.0512f, 0.0512f };
		Text_SetFromSIS(text, 0);
		data->description_text = text;
	} else if (!index_changed && !value_changed) {
		return;
	}

	const auto index = MenuSelectedIndex;
	const auto value = MenuSelectedValue;

	// Update rules from extra rotator values
	if (value_changed && index == ExtraRule_Latency)
		GetGameRules()->latency = (latency_mode)value;

	if (value_changed && index == ExtraRule_Widescreen)
		GetGameRules()->widescreen = value;

	// Stage mods description needs accented E
	if (index == ExtraRule_StageMods && !IsLanguageUS())
		text->sis_id = 1;
	else
		text->sis_id = 0;

	switch (index) {
	case ExtraRule_StockMatchTimeLimit:
		// Has to be hard coded because ExtraRuleDescriptions gets clobbered by extra
		// rotator bounds
		Text_SetFromSIS(text, 0x37);
		break;
	case ExtraRule_Pause:
		if (value)
			Text_SetFromSIS(text, 0x3B);
		else
			text->data = pause_auto_description.data();
		break;
	case ExtraRule_StageMods:      text->data = stage_mod_descriptions[value]; break;
	case ExtraRule_ControllerFix:  text->data = ucf_type_descriptions[value]; break;
	case ExtraRule_Latency:        text->data = latency_descriptions[value]; break;
	case ExtraRule_Widescreen:     text->data = widescreen_descriptions[value]; break;
	}
}
#include "controls/config.h"
#include "hsd/archive.h"
#include "hsd/dobj.h"
#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "hsd/memory.h"
#include "hsd/mobj.h"
#include "hsd/tobj.h"
#include "latency/polling.h"
#include "melee/menu.h"
#include "melee/preferences.h"
#include "melee/rules.h"
#include "melee/scene.h"
#include "melee/text.h"
#include "rules/saved_config.h"
#include "rules/values.h"
#include "util/texture_swap.h"
#include "util/mempool.h"
#include "util/meta.h"
#include "util/patch_list.h"
#include "util/melee/text_builder.h"
#include <gctypes.h>
#include <ogc/gx.h>

#include "resources/rules/pause_values.tex.h"
#ifdef FULL_SSS_ROTATOR
#include "resources/rules/old/sss.tex.h"
#include "resources/rules/old/sss_values.tex.h"
#else
#include "resources/rules/sss.tex.h"
#include "resources/rules/sss_values.tex.h"
#endif
#include "resources/rules/controls.tex.h"
#include "resources/rules/controls_values.tex.h"
#ifdef UCF_ROTATOR
#include "resources/rules/controller_fix.tex.h"
#include "resources/rules/controller_fix_values.tex.h"
#endif
#include "resources/rules/latency.tex.h"
#include "resources/rules/latency_values.tex.h"
#include "resources/rules/widescreen.tex.h"
#include "resources/rules/widescreen_values.tex.h"

struct ExtraRulesMenuData {
	u8 menu_type;
	u8 selected;
	union {
		struct {
			u8 stock_time_limit;
			// Pause is moved up a row
			u8 pause;
			sss_type stage_mods;
			controls_type controls;
			ucf_type controller_fix;
			latency_mode latency;
			widescreen_mode widescreen;
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

// Stage selection values model
extern "C" ArchiveModel MenMainCursorRl05_Top;

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
		text_builder::type_speed<0, 0>(),
		text_builder::fit(),
		text_builder::text<line1>(),
		text_builder::end_fit(),
		text_builder::br(),
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
		text_builder::offset<0, -10>(),
		text_builder::br(),
		text_builder::type_speed<0, 0>(),
		text_builder::fit(),
		text_builder::text<line1>(),
		text_builder::end_fit(),
		text_builder::reset_scale(),
		text_builder::end_color());
}

constexpr auto pause_auto_description =
	make_description_text<"Players will not be able to",
	                      "pause during 4-stock matches.">();

constexpr auto stage_mod_descriptions = multi_array {
#ifndef FULL_SSS_ROTATOR
	make_description_text<"Use 1.03's stage select screen",
	                      "and play with stage modifications.">(),
	make_description_text<"Use the original stage select screen",
	                      "and play without stage modifications.">()
#else
	make_description_text<"Modify all tournament legal",
	                      "stages except for Battlefield.">(),
	make_description_text<"Use the original stage select screen",
	                      "and play without stage modifications.">(),
	make_description_text<"Play without stage modifications.">(),
	make_description_text<u"Freeze Final Destination and",
	                      u"Pokémon Stadium.">()
#endif
};

constexpr auto controls_descriptions = multi_array {
	make_description_text<"Allow the use of Z jump.">(),
	make_description_text<"Allow the use of Z jump and",
	                      "perfect angles.">(),
	make_description_text<"Allow the use of all controls",
	                      "except perfect angles.">(),
	make_description_text<"Allow the use of all controls.">()
};

constexpr auto ucf_type_descriptions = multi_array {
	make_description_text<"Use the 1.03 controller fix.">(),
	make_description_text<"Use Universal Controller Fix.">()
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
		              "ratio.">(),
	make_description_text<"Play with the normal aspect",
		              "ratio on a widescreen display.">()
};

static mempool pool;
static texture_swap *widescreen_text;

PATCH_LIST(
	// Swap text for pause and friendly fire
	std::pair { &ExtraRuleTextAnimFrames[ExtraRule_Pause].unselected,        24.f },
	std::pair { &ExtraRuleTextAnimFrames[ExtraRule_Pause].selected,          25.f },
	std::pair { &ExtraRuleTextAnimFrames[ExtraRule_FriendlyFire].unselected, 22.f },
	std::pair { &ExtraRuleTextAnimFrames[ExtraRule_FriendlyFire].selected,   23.f },
	// Add a value model for index 5
	// nop
	std::pair { Menu_SetupExtraRulesMenu+0x558,                       0x60000000u },
	// Allow scrolling left/right on index 5
	// nop
	std::pair { Menu_ExtraRulesMenuInput+0x394,                       0x60000000u },
	// Don't allow pressing A on index 5
	// b 0x3CC
	std::pair { Menu_ExtraRulesMenuInput+0x50,                        0x480003CCu },
	// Skip index 5 checks
	// b 0x14
	std::pair { Menu_UpdateExtraRuleDisplay+0x218,                    0x48000014u },
	// nop
	std::pair { Menu_UpdateExtraRuleDisplay+0x2B8,                    0x60000000u },
	// b 0x28
	std::pair { Menu_UpdateExtraRuleDisplay+0x330,                    0x48000028u },
	// nop
	std::pair { Menu_UpdateExtraRuleDisplay+0x388,                    0x60000000u },
	std::pair { Menu_UpdateExtraRuleDisplay+0x38C,                    0x60000000u },
	// nop
	std::pair { Menu_UpdateExtraRuleDisplay+0x4FC,                    0x60000000u },
	// b 0x20
	std::pair { Menu_UpdateExtraRuleDisplay+0x510,                    0x48000020u },
	// b 0x24
	std::pair { Menu_GetExtraRuleValueAnimLoop+0x10,                  0x48000024u },
	// b 0x8
	std::pair { Menu_UpdateExtraRuleValueAnim+0x1C,                   0x48000008u },

	// Add a 7th rotator to extra rules
	std::pair { &MenuTypeDataTable[MenuType_ExtraRules].option_count,        (u8)7 },
	// cmpwi r31, 7
	std::pair { Menu_SetupExtraRulesMenu+0x388,                       0x2C1F0007u },
	// li r0, 6
	std::pair { Menu_ExtraRulesMenuInput+0x214,                       0x38000006u },
	// cmpwi r3, 6
	std::pair { Menu_ExtraRulesMenuInput+0x2D8,                       0x2C030006u },
	// cmplwi r5, 6
	std::pair { Menu_ExtraRulesMenuInput+0x390,                       0x28050006u },

	// Move data->state from 0x08 to 0x09
	// stb r16, 0x9(r25)
	std::pair { Menu_SetupExtraRulesMenu+0x220,                       0x9A190009u },
	// lbz r0, 0x9(r25)
	std::pair { Menu_SetupExtraRulesMenu+0x27C,                       0x88190009u },
	// lbz r0, 0x9(r31)
	std::pair { Menu_ExtraRulesMenuThink+0x2C,                        0x881F0009u },
	// stb r0, 0x9(r31)
	std::pair { Menu_ExtraRulesMenuThink+0x6C,                        0x981F0009u },
	// stb r0, 0x9(r31)
	std::pair { Menu_ExtraRulesMenuThink+0x78,                        0x981F0009u },
	// lbz r0, 0x9(r31)
	std::pair { Menu_ExtraRulesMenuThink+0x7C,                        0x881F0009u },
	// lbz r0, 0x9(r31)
	std::pair { Menu_ExtraRulesMenuThink+0xDC,                        0x881F0009u },
	// lbz r0, 0x9(r31)
	std::pair { Menu_ExtraRulesMenuThink+0x104,                       0x881F0009u },
	// lbz r0, 0x9(r31)
	std::pair { Menu_ExtraRulesMenuThink+0x170,                       0x881F0009u },
	// stb r0, 0x9(r31)
	std::pair { Menu_ExtraRulesMenuThink+0x1A0,                       0x981F0009u },
	// lbz r0, 0x9(r31)
	std::pair { Menu_ExtraRulesMenuThink+0x1BC,                       0x881F0009u },

	// Increase ExtraRulesMenuData size from 0xE0 to 0xFC
	// li r3, 0xE4
	std::pair { Menu_SetupExtraRulesMenu+0x180,                       0x386000FCu },

	// Move data->description_text from 0xDC to 0xF8
	// stw r24, 0xE0(r25)
	std::pair { Menu_SetupExtraRulesMenu+0x224,                       0x931900F8u },
	// lwz r3, 0xE0(r25)
	std::pair { Menu_SetupExtraRulesMenu+0x72C,                       0x807900F8u },
	// stw r0, 0xE0(r25)
	std::pair { Menu_SetupExtraRulesMenu+0x748,                       0x901900F8u },
	// stw r3, 0xE0(r25)
	std::pair { Menu_SetupExtraRulesMenu+0x79C,                       0x907900F8u },

	// Always use count of 7 to populate value_jobj_trees to avoid overread from extra rotator
	// li r0, 7
	std::pair { Menu_SetupExtraRulesMenu+0x600,                       0x38000007u }
);

extern "C" bool is_extra_rule_visible(int index)
{
	switch (index) {
	case ExtraRule_Latency:       return !is_faster_melee();
#ifndef UCF_ROTATOR
	case ExtraRule_ControllerFix: return false;
#endif
#ifdef TOURNAMENT
	case ExtraRule_Controls:      return false;
#endif
	default:                      return true;
	}
}

static int get_jobj_index(int index)
{
	// Fix up indices for hidden rotators
	for (auto i = index - 1; i >= 0; i--) {
		if (!is_extra_rule_visible(i))
			index--;
	}

	return index;
}

static void replace_toggle_texture(ExtraRulesMenuData *data, int index, u8 *tex_data,
                                   bool force_allocation = false)
{
	if (!is_extra_rule_visible(index))
		return;

	auto *tobj = data->value_jobj_trees[index].tree[1]->u.dobj->mobj->tobj;

	// Force a new texture buffer to be allocated for duplicate models
	if (force_allocation)
		tobj->imagedesc = pool.add_texture_swap(tex_data)->image;
	else
		pool.add_texture_swap(tex_data, tobj->imagedesc);
}

static void fix_widescreen_text(ExtraRulesMenuData *data)
{
	auto *cursor = data->jobj_tree.rules[get_jobj_index(ExtraRule_Widescreen)]->child;
	auto *text = HSD_JObjGetFromTreeByIndex(cursor, 7);
	text->u.dobj->mobj->tobj->imagedesc = widescreen_text->image;
}

static void set_to_rotator(ExtraRulesMenuData *data, int index)
{
	if (!is_extra_rule_visible(index))
		return;

	auto *cursor = data->jobj_tree.rules[get_jobj_index(index)]->child;
	auto [background, arrow, scroll] = HSD_JObjGetFromTreeTuple<6, 8, 13>(cursor);

	// Show black value background
	HSD_JObjClearFlagsAll(background, HIDDEN);

	if (data->selected == index) {
		// Show scroll arrows
		HSD_JObjClearFlagsAll(scroll, HIDDEN);
		// Hide arrow used for portals
		HSD_JObjSetFlagsAll(arrow, HIDDEN);
	}
}

static void fix_value_position(ExtraRulesMenuData *data, int index)
{
	if (!is_extra_rule_visible(index))
		return;

	// Set to proper position for additional rules value
	auto *jobj = data->value_jobj_trees[index].tree[1];
	jobj->position.x = 4.8f;
	HSD_JObjSetMtxDirty(jobj);
}

static u8 get_model_max_value(int index)
{
	// Use max value corresponding to 5-value model for controls
	if (index == ExtraRule_Controls)
		return 4;
	else
		return ExtraRuleValueBounds[index].max;
}

static void set_value_anim(ExtraRulesMenuData *data, int index)
{
	if (!is_extra_rule_visible(index))
		return;

	// Set initial value anim frame with support for more than 3 values
	auto *jobj = data->value_jobj_trees[index].tree[0];
	const auto value = data->values[index];

	if (index == ExtraRule_Controls) {
		// Shift value for controls to allow for 4-value rotator on 5-value model
		HSD_JObjReqAnimAll(jobj, RuleValueAnimLoops[value].start);
	} else {
		const auto anim_index = value == 0 ? get_model_max_value(index) : value - 1;
		HSD_JObjReqAnimAll(jobj, RuleValueAnimLoops[anim_index].start);
	}

	HSD_JObjAnimAll(jobj);
}

static void fix_rule_scale(ExtraRulesMenuData *data, int index)
{
	if (!is_extra_rule_visible(index))
		return;

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
	pool.add_texture_swap(sss_tex_data,             rule_names->texanim->imagetbl[ 9]);
#ifndef TOURNAMENT
	pool.add_texture_swap(controls_tex_data,        rule_names->texanim->imagetbl[11]);
#endif
#ifdef UCF_ROTATOR
	pool.add_texture_swap(controller_fix_tex_data,  rule_names->texanim->imagetbl[12]);
#endif
	pool.add_texture_swap(latency_tex_data,         rule_names->texanim->imagetbl[13]);
	widescreen_text = pool.add_texture_swap(widescreen_tex_data);
}

extern "C" ArchiveModel *select_extra_rule_model(u32 index)
{
	return std::array {
		&MenMainCursorTr01_Top, // Stock Match Time Limit
		&MenMainCursorTr03_Top, // Pause
#ifdef FULL_SSS_ROTATOR
		&MenMainCursorRl01_Top, // Stage Select Screen
#else
		&MenMainCursorTr03_Top, // Stage Select Screen
#endif
		&MenMainCursorRl05_Top, // Controls
		&MenMainCursorTr03_Top, // Controller Fix
		&MenMainCursorTr04_Top, // Latency
		&MenMainCursorTr04_Top, // Widescreen
	}[index];
}

extern "C" void hook_Menu_UpdateExtraRuleDescriptionText(HSD_GObj *gobj,
                                                         bool index_changed, bool value_changed)
{
	auto *data = gobj->get<ExtraRulesMenuData>();
	auto *text = data->description_text;

	// Fix all rule anims since changes will have been overridden
	fix_rule_anims(data);

	// Manually apply widescreen rotator texture since there's normally no 7th texture
	fix_widescreen_text(data);

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
		GetGameRules()->widescreen = (widescreen_mode)value;

	// Stage mods description needs accented E
	if (index == ExtraRule_StageMods && !IsLanguageUS())
		text->sis_id = 1;
	else
		text->sis_id = 0;

	switch (index) {
	case ExtraRule_StockMatchTimeLimit:
		// Has to be hard coded because ExtraRuleDescriptions gets clobbered by extra
		// rotator bounds
#ifdef PAL
		Text_SetFromSIS(text, 0x35);
#else
		Text_SetFromSIS(text, 0x37);
#endif
		break;
	case ExtraRule_Pause:
		if (value) {
#ifdef PAL
			Text_SetFromSIS(text, 0x39);
#else
			Text_SetFromSIS(text, 0x3B);
#endif
		} else {
			text->data = pause_auto_description.data();
		}
		break;
	case ExtraRule_StageMods:      text->data = stage_mod_descriptions[value];  break;
	case ExtraRule_Controls:       text->data = controls_descriptions[value];   break;
	case ExtraRule_ControllerFix:  text->data = ucf_type_descriptions[value];   break;
	case ExtraRule_Latency:        text->data = latency_descriptions[value];    break;
	case ExtraRule_Widescreen:     text->data = widescreen_descriptions[value]; break;
	}
}

extern "C" HSD_GObj *orig_Menu_SetupExtraRulesMenu(u8 state);
extern "C" HSD_GObj *hook_Menu_SetupExtraRulesMenu(u8 state)
{
	// Ensure all of the options always get populated
	FeatureUnlockMask = 0xFF;

	auto *gobj = orig_Menu_SetupExtraRulesMenu(state);
	auto *data = gobj->get<ExtraRulesMenuData>();

	// Initialize extra rotators
	data->latency = GetGameRules()->latency;
	set_value_anim(data, ExtraRule_Latency);
	data->widescreen = GetGameRules()->widescreen;
	set_value_anim(data, ExtraRule_Widescreen);

	// Free assets on menu exit
	gobj->user_data_remove_func = pool_free;

	// Free previous assets
	if (pool.inc_ref() != 0)
		pool.reset();

	load_textures();

	// Replace rule value textures
	replace_toggle_texture(data, ExtraRule_Pause,         pause_values_tex_data);
#ifdef FULL_SSS_ROTATOR
	replace_toggle_texture(data, ExtraRule_StageMods,     sss_values_tex_data);
#else
	replace_toggle_texture(data, ExtraRule_StageMods,     sss_values_tex_data, true);
#endif
#ifndef TOURNAMENT
	replace_toggle_texture(data, ExtraRule_Controls,      controls_values_tex_data);
#endif
#ifdef UCF_ROTATOR
	replace_toggle_texture(data, ExtraRule_ControllerFix, controller_fix_values_tex_data, true);
#endif
	replace_toggle_texture(data, ExtraRule_Latency,       latency_values_tex_data);
	replace_toggle_texture(data, ExtraRule_Widescreen,    widescreen_values_tex_data), true;

	// Make Rl01 and Rl05 use proper additional rules position
	fix_value_position(data, ExtraRule_StageMods);
	fix_value_position(data, ExtraRule_Controls);

	// Set anim frame correctly for 4/5-value model
	set_value_anim(data, ExtraRule_StageMods);
	set_value_anim(data, ExtraRule_Controls);

	// Use rotator for latency (replacing random stage select)
	set_to_rotator(data, ExtraRule_Latency);
	set_value_anim(data, ExtraRule_Latency);

	// Manually apply widescreen rotator texture since there's normally no 7th texture
	fix_widescreen_text(data);

	// Initialize description with custom handling
	hook_Menu_UpdateExtraRuleDescriptionText(gobj, true, false);

	return gobj;
}

extern "C" bool orig_Menu_IsExtraRuleVisible(u8 index);
extern "C" bool hook_Menu_IsExtraRuleVisible(u8 index)
{
	return is_extra_rule_visible(index);
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

	while (!is_extra_rule_visible(MenuSelectedIndex)) {
		// Skip over the nonexistent rotator
		if (buttons & MenuButton_Up)
			MenuSelectedIndex--;
		else
			MenuSelectedIndex++;

		const auto *data = ExtraRulesMenuGObj->get<ExtraRulesMenuData>();
		MenuSelectedValue = data->values[MenuSelectedIndex];
	}
}

extern "C" const HSD_AnimLoop &orig_Menu_GetExtraRuleValueAnimLoop(u8 index, u8 value,
                                                                   bool scroll_right);
extern "C" const HSD_AnimLoop &hook_Menu_GetExtraRuleValueAnimLoop(u8 index, u8 value,
                                                                   bool scroll_right)
{
	if (index != ExtraRule_StageMods && index != ExtraRule_Controls)
		return orig_Menu_GetExtraRuleValueAnimLoop(index, value, scroll_right);

	// Use max value corresponding to 5-value model for controls
	const auto max = get_model_max_value(index);

	// Shift value for controls to allow for 4-value rotator on 5-value model
	if (index == ExtraRule_Controls)
		value++;

	if (!scroll_right)
		return RuleValueAnimLoops[max - value + 5];
	else if (value == 0)
		return RuleValueAnimLoops[max];
	else
		return RuleValueAnimLoops[value - 1];
}

extern "C" void orig_Menu_UpdateExtraRuleValueAnim(HSD_GObj *gobj, HSD_JObj *jobj, u8 index);
extern "C" void hook_Menu_UpdateExtraRuleValueAnim(HSD_GObj *gobj, HSD_JObj *jobj, u8 index)
{
	if (index != ExtraRule_StageMods && index != ExtraRule_Controls)
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
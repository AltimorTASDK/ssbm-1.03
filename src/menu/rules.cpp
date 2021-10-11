#include "hsd/archive.h"
#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "hsd/mobj.h"
#include "hsd/tobj.h"
#include "melee/rules.h"
#include "melee/text.h"
#include "resources/rules/ledge_grab_limit.tex.h"
#include "resources/rules/air_time_limit.tex.h"
#include "rules/values.h"
#include "util/compression.h"
#include "util/melee/text_builder.h"
#include <gctypes.h>

// Matanim used to cycle through rule text
extern "C" ArchiveModel MenMainCursorRl_Top;

extern "C" ArchiveModel MenMainNmRl_Top;

extern "C" u16 SelectedRuleIndex;
extern "C" u8 SelectedRuleValue;

extern "C" struct {
	ArchiveModel *mode;
	ArchiveModel *stock_count;
	ArchiveModel *handicap;
	ArchiveModel *damage_ratio;
	ArchiveModel *stage_selection;
} MenuRuleValueModels;

struct RulesMenuData {
	u8 match_type_major;
	u8 selected;
	u8 mode;
	u8 time_limit;
	u8 handicap;
	u8 damage_ratio;
	u8 pad007;
	u8 pad008;
	u8 stock_count;
	u8 submenu;
	u8 pad00B;
	struct {
		HSD_JObj *root1;
		HSD_JObj *root2;
		HSD_JObj *root3;
		HSD_JObj *rules[Rule_Max];
	} jobj_tree;
	struct {
		HSD_JObj *tree[9];
	} value_jobj_trees[Rule_Max];
	 Text *description_text;
};

constexpr auto lgl_description = text_builder::build(
	text_builder::kern(),
	text_builder::left(),
	text_builder::color<170, 170, 170>(),
	text_builder::textbox<179, 179>(),
	text_builder::unk06<0, 0>(),
	text_builder::fit(),
	text_builder::ascii<"Set the LGL in the event of">(),
	text_builder::end_fit(),
	text_builder::br(),
	text_builder::fit(),
	text_builder::ascii<"a time-out.">(),
	text_builder::end_fit(),
	text_builder::end_textbox(),
	text_builder::end_color());

constexpr auto atl_description = text_builder::build(
	text_builder::kern(),
	text_builder::left(),
	text_builder::color<170, 170, 170>(),
	text_builder::textbox<179, 179>(),
	text_builder::unk06<0, 0>(),
	text_builder::fit(),
	text_builder::ascii<"Set the ATL in the event of">(),
	text_builder::end_fit(),
	text_builder::br(),
	text_builder::fit(),
	text_builder::ascii<"a time-out.">(),
	text_builder::end_fit(),
	text_builder::end_textbox(),
	text_builder::end_color());

static void replace_value_jobj(HSD_JObj *parent, HSD_JObj **jobj_tree)
{
	// Remove existing value jobj
	HSD_JObjRemoveAll(parent->child->next);

	// Create new value jobj
	const auto *value_model = MenuRuleValueModels.stock_count;
	auto *value_jobj = HSD_JObjFromArchiveModel(value_model);
	HSD_JObjReqAnimAll(value_jobj, 0.f);
	HSD_JObjAnimAll(value_jobj);
	
	// Update the stored jobj tree for the new jobj
	HSD_JObjGetTree<9>(value_jobj, jobj_tree);
	
	// Parent in the new value jobj
	HSD_JObjAddChild(parent, value_jobj);
}

static void replace_counter_jobj(RulesMenuData *rules_data, int index)
{
	auto *parent = rules_data->jobj_tree.rules[index];
	auto **jobj_tree = rules_data->value_jobj_trees[index].tree;
	replace_value_jobj(parent, jobj_tree);

	// Hide ":"
	HSD_JObjSetFlagsAll(jobj_tree[4], HIDDEN);

	// Digits
	HSD_JObjAddChild(jobj_tree[7], HSD_JObjFromArchiveModel(&MenMainNmRl_Top));
	HSD_JObjAddChild(jobj_tree[8], HSD_JObjFromArchiveModel(&MenMainNmRl_Top));
}

static void replace_timer_jobj(RulesMenuData *rules_data, int index)
{
	auto *parent = rules_data->jobj_tree.rules[index];
	auto **jobj_tree = rules_data->value_jobj_trees[index].tree;
	replace_value_jobj(parent, jobj_tree);

	// Minutes
	HSD_JObjAddChild(jobj_tree[2], HSD_JObjFromArchiveModel(&MenMainNmRl_Top));
	HSD_JObjAddChild(jobj_tree[3], HSD_JObjFromArchiveModel(&MenMainNmRl_Top));

	// Seconds
	HSD_JObjAddChild(jobj_tree[5], HSD_JObjFromArchiveModel(&MenMainNmRl_Top));
	HSD_JObjAddChild(jobj_tree[6], HSD_JObjFromArchiveModel(&MenMainNmRl_Top));
}

static void update_value_digit(HSD_JObj *jobj, u32 digit)
{
	HSD_JObjReqAnimAll(jobj, (float)digit);
	HSD_JObjAnimAll(jobj);
}

static void show_timer_value(HSD_JObj **jobj_tree, bool show)
{
	// Show/hide digits
	for (auto i = 2; i < 9; i++) {
		if (i == 4)
			continue;

		if (show)
			HSD_JObjClearFlagsAll(jobj_tree[i], HIDDEN);
		else
			HSD_JObjSetFlagsAll(jobj_tree[i], HIDDEN);
	}
	
	// Swap joint 4 between ":" or "NONE" with mat anim
	HSD_JObjReqAnimAll(jobj_tree[4], show ? 0.f : 1.f);
	HSD_JObjAnimAll(jobj_tree[4]);
}

static void show_counter_value(HSD_JObj **jobj_tree, bool show)
{
	show_timer_value(jobj_tree, show);

	// Hide ":" when not displaying "NONE"
	if (!show)
		HSD_JObjClearFlagsAll(jobj_tree[4], HIDDEN);
	else
		HSD_JObjSetFlagsAll(jobj_tree[4], HIDDEN);
}

static void update_lgl_value(HSD_GObj *menu_obj, u32 value)
{
	auto *rules_data = menu_obj->get<RulesMenuData>();
	auto **jobj_tree = rules_data->value_jobj_trees[Rule_LedgeGrabLimit].tree;
	
	if (value == 0) {
		show_counter_value(jobj_tree, false);
		return;
	}

	show_counter_value(jobj_tree, true);

	const auto time = ledge_grab_limit_values[value];

	update_value_digit(jobj_tree[7], time / 10);
	update_value_digit(jobj_tree[8], time % 10);
}

static void update_atl_value(HSD_GObj *menu_obj, u32 value)
{
	auto *rules_data = menu_obj->get<RulesMenuData>();
	auto **jobj_tree = rules_data->value_jobj_trees[Rule_AirTimeLimit].tree;

	if (value == 0) {
		show_timer_value(jobj_tree, false);
		return;
	}

	show_timer_value(jobj_tree, true);

	const auto time = air_time_limit_values[value];
	const auto minutes = time / 60;
	const auto seconds = time % 60;

	update_value_digit(jobj_tree[2], minutes / 10);
	update_value_digit(jobj_tree[3], minutes % 10);
	update_value_digit(jobj_tree[5], seconds / 10);
	update_value_digit(jobj_tree[6], seconds % 10);
}

extern "C" HSD_GObj *orig_Menu_SetupRuleMenu(u32 entry_point);
extern "C" HSD_GObj *hook_Menu_SetupRuleMenu(u32 entry_point)
{
	auto *menu_obj = orig_Menu_SetupRuleMenu(entry_point);
	auto *rules_data = menu_obj->get<RulesMenuData>();

	// Replace rule name textures
	const auto *matanim = MenMainCursorRl_Top.matanim_joint->child->child->next->matanim;
	matanim->texanim->imagetbl[3]->img_ptr = rle_decode(ledge_grab_limit_tex_data);
	matanim->texanim->imagetbl[4]->img_ptr = rle_decode(air_time_limit_tex_data);
	
	// Replace rule value models
	replace_counter_jobj(rules_data, Rule_LedgeGrabLimit);
	replace_timer_jobj(rules_data, Rule_AirTimeLimit);
	
	// Display initial values
	update_lgl_value(menu_obj, rules_data->handicap);
	update_atl_value(menu_obj, rules_data->damage_ratio);
	
	return menu_obj;
}

extern "C" void orig_Menu_UpdateRuleValue(HSD_GObj *menu_obj, HSD_JObj *jobj, u8 index, u32 value);
extern "C" void hook_Menu_UpdateRuleValue(HSD_GObj *menu_obj, HSD_JObj *jobj, u8 index, u32 value)
{
	if (index == Rule_LedgeGrabLimit)
		update_lgl_value(menu_obj, value);
	else if (index == Rule_AirTimeLimit)
		update_atl_value(menu_obj, value);
	else
		orig_Menu_UpdateRuleValue(menu_obj, jobj, index, value);
}

extern "C" void orig_Menu_CreateRuleDescriptionText(RulesMenuData *rules_data, int rule, int value);
extern "C" void hook_Menu_CreateRuleDescriptionText(RulesMenuData *rules_data, int rule, int value)
{
	orig_Menu_CreateRuleDescriptionText(rules_data, rule, value);
	
	if (rule == Rule_LedgeGrabLimit)
		rules_data->description_text->data = lgl_description.data();
	else if (rule == Rule_AirTimeLimit)
		rules_data->description_text->data = atl_description.data();
}
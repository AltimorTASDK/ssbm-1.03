#include "hsd/archive.h"
#include "hsd/dobj.h"
#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "hsd/mobj.h"
#include "hsd/tobj.h"
#include "melee/menu.h"
#include "melee/rules.h"
#include "melee/text.h"
#include "util/compression.h"
#include "util/patch_list.h"
#include "util/melee/text_builder.h"
#include <gctypes.h>

#include "resources/rules/controller_fix.tex.h"
#include "resources/rules/controller_fix_values.tex.h"

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
		u8 low_latency;
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

// Controller fix values
extern "C" ArchiveModel MenMainCursorTr03_Top;

extern "C" struct {
	f32 unselected;
	f32 selected;
} ExtraRuleTextAnimFrames[6];

extern "C" struct {
	u8 values[3];
} ExtraRuleDescriptions[6];

constexpr auto hax_fix_description = text_builder::build(
	text_builder::kern(),
	text_builder::left(),
	text_builder::color<170, 170, 170>(),
	text_builder::textbox<179, 179>(),
	text_builder::unk06<0, 0>(),
	text_builder::fit(),
	text_builder::ascii<"Maximize shield drop's range and fix">(),
	text_builder::end_fit(),
	text_builder::br(),
	text_builder::fit(),
	text_builder::ascii<"1.0 cardinal and dash out of crouch.">(),
	text_builder::end_fit(),
	text_builder::end_textbox(),
	text_builder::end_color());

constexpr auto ucf_fix_description = text_builder::build(
	text_builder::kern(),
	text_builder::left(),
	text_builder::color<170, 170, 170>(),
	text_builder::textbox<179, 179>(),
	text_builder::unk06<0, 0>(),
	text_builder::fit(),
	text_builder::ascii<"Reduce shield drop's range and remove 1.0">(),
	text_builder::end_fit(),
	text_builder::br(),
	text_builder::fit(),
	text_builder::ascii<"cardinal and dash out of crouch fixes.">(),
	text_builder::end_fit(),
	text_builder::end_textbox(),
	text_builder::end_color());

const auto patches = patch_list {
	// Swap text for pause and friendly fire
	std::pair { (char*)&ExtraRuleTextAnimFrames[ExtraRule_Pause].unselected,        24.f },
	std::pair { (char*)&ExtraRuleTextAnimFrames[ExtraRule_Pause].selected,          25.f },
	std::pair { (char*)&ExtraRuleTextAnimFrames[ExtraRule_FriendlyFire].unselected, 22.f },
	std::pair { (char*)&ExtraRuleTextAnimFrames[ExtraRule_FriendlyFire].selected,   23.f },

	// Swap description for pause and friendly fire
	std::pair { (char*)&ExtraRuleDescriptions[ExtraRule_Pause].values[0],           (u8)0x3A },
	std::pair { (char*)&ExtraRuleDescriptions[ExtraRule_Pause].values[1],           (u8)0x3B },
};

extern "C" HSD_GObj *orig_Menu_SetupExtraRulesMenu(u8 state);
extern "C" HSD_GObj *hook_Menu_SetupExtraRulesMenu(u8 state)
{
	auto *menu_obj = orig_Menu_SetupExtraRulesMenu(state);
	auto *data = menu_obj->get<ExtraRulesMenuData>();

	// Replace rule name textures
	const auto *rule_names = MenMainCursorRl_Top.matanim_joint->child->child->next->matanim;
	rule_names->texanim->imagetbl[9]->img_ptr = decompress(controller_fix_tex_data);
	
	// Replace controller fix value texture
	auto *controller_fix_values = data->value_jobj_trees[ExtraRule_ControllerFix].tree[1];
	auto **controller_fix_image = &controller_fix_values->u.dobj->mobj->tobj->imagedesc;
	auto *new_desc = new HSD_ImageDesc;
	*new_desc = **controller_fix_image;
	new_desc->img_ptr = decompress(controller_fix_values_tex_data);
	*controller_fix_image = new_desc;
		
	return menu_obj;
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

	switch (MenuSelectedIndex) {
	case ExtraRule_ControllerFix:
		switch (MenuSelectedValue) {
		case (u8)ucf_type::hax: text->data = hax_fix_description.data(); break;
		case (u8)ucf_type::ucf: text->data = ucf_fix_description.data(); break;
		}
		break;
	}
}
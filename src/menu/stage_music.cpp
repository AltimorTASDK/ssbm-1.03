#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "hsd/mobj.h"
#include "hsd/tobj.h"
#include "melee/stage.h"
#include "melee/text.h"
#include "util/compression.h"
#include "util/math.h"
#include "util/meta.h"
#include "util/melee/text_builder.h"
#include <cstring>
#include <gctypes.h>

#include "resources/music/music_stages.tex.h"
#include "resources/music/music_stages_mask.tex.h"

struct ItemMenuData {
	u8 match_type_major;
	u8 selected;
	u8 toggles[31];
	union {
		u8 item_frequency;
		u8 selected_stage;
	};
	char pad022;
	u8 state;
	HSD_JObj *jobj_tree[7];
	Text *text_left;
	Text *text_right;
};

extern "C" u16 MenuSelectedIndex;
extern "C" u8 MenuSelectedValue;

extern "C" ArchiveModel MenMainConIs_Top;

extern "C" HSD_JObj *Menu_GetItemToggle(ItemMenuData *data, u16 index);

int bgm_selection[6] = { -1, -1, -1, -1, -1, -1 };

template<string_literal str, u16 scale = 138>
static constexpr auto item_text()
{
	return array_cat(
		text_builder::textbox<scale, 138>(),
		text_builder::ascii<str>(),
		text_builder::end_textbox(),
		text_builder::br());
}

constexpr auto text_left = text_builder::build(
	text_builder::right(),
	text_builder::kern(),
	item_text<"Balloon Fight">(),
	item_text<"Big Blue">(),
	item_text<"Brinstar">(),
	item_text<"Brinstar Depths">(),
	item_text<"Corneria">(),
	item_text<"Dr. Mario">(),
	item_text<"Earthbound">(),
	item_text<"Fire Emblem">(),
	item_text<"Flat Zone">(),
	item_text<"Great Bay">(),
	item_text<"Green Greens">(),
	item_text<"Temple">(),
	item_text<"Icicle Mountain">(),
	item_text<"Jungle Japes">(),
	item_text<"Kongo Jungle">(),
	item_text<"Kongo Jungle N64">());

constexpr auto text_right = text_builder::build(
	text_builder::right(),
	text_builder::kern(),
	item_text<"Mach Rider">(),
	item_text<"Metal Battle">(),
	item_text<"Mother">(),
	item_text<"Mother 2">(),
	item_text<"Mushroom Kingdom", 126>(),
	item_text<"Mushroom Kingdom II", 114>(),
	item_text<"Mute City">(),
	item_text<u"Poké Floats">(),
	item_text<"Princess Peach's Castle", 102>(),
	item_text<"Rainbow Cruise">(),
	item_text<"Saria's Theme">(),
	item_text<"Super Mario Bros. 3", 124>(),
	item_text<"Venom">(),
	item_text<"Yoshi's Island">(),
	item_text<"Yoshi's Island N64">(),
	item_text<"Custom Music">());
	
constexpr int bgm_ids[] = {
	2,
	3,
	97,
	51,
	6,
	7,
	31,
	1,
	30,
	34,
	35,
	75,
	40,
	50,
	33,
	57,
	
	55,
	80,
	60,
	61,
	41,
	43,
	56,
	65,
	4,
	66,
	74,
	77,
	84,
	95,
	59
};
	
static void set_toggle(ItemMenuData *data, u16 index, bool toggle)
{
	data->toggles[index] = toggle;

	auto *jobj = Menu_GetItemToggle(data, index);
	auto *on_off_switch = HSD_JObjGetFromTreeByIndex(jobj, 2);

	if (toggle)
		HSD_JObjReqAnimAll(on_off_switch, 1.f);
	else
		HSD_JObjReqAnimAll(on_off_switch, 0.f);

	HSD_JObjAnimAll(on_off_switch);
}

static void change_stage(ItemMenuData *data, u8 stage)
{
	// Copy selection for stage
	for (u16 i = 0; i < 31; i++)
		set_toggle(data, i, i == bgm_selection[stage]);
}

extern "C" void orig_Menu_UpdateItemDisplay(HSD_GObj *gobj, bool index_changed, bool value_changed);
extern "C" void hook_Menu_UpdateItemDisplay(HSD_GObj *gobj, bool index_changed, bool value_changed)
{
	orig_Menu_UpdateItemDisplay(gobj, index_changed, value_changed);
	
	auto *data = gobj->get<ItemMenuData>();
	
	if (MenuSelectedIndex >= 31 && value_changed) {
		change_stage(data, MenuSelectedValue);
		return;
	}

	// Hide selection indicator
	auto *jobj = Menu_GetItemToggle(data, MenuSelectedIndex);
	HSD_JObjSetFlagsAll(HSD_JObjGetFromTreeByIndex(jobj, 4), HIDDEN);
	
	if (!value_changed)
		return;
		
	if (MenuSelectedValue)
		bgm_selection[data->selected_stage] = MenuSelectedIndex;
	else
		bgm_selection[data->selected_stage] = -1;
		
	for (u16 i = 0; i < 31; i++) {
		// Disable all other songs
		if (i != MenuSelectedIndex)
			set_toggle(data, i, false);
	}
}

extern "C" void orig_Menu_SetupItemToggles(HSD_GObj *gobj);
extern "C" void hook_Menu_SetupItemToggles(HSD_GObj *gobj)
{
	orig_Menu_SetupItemToggles(gobj);

	auto *data = gobj->get<ItemMenuData>();

	for (u16 i = 0; i < 31; i++) {
		auto *jobj = Menu_GetItemToggle(data, i);
		// Hide item image
		HSD_JObjSetFlagsAll(HSD_JObjGetFromTreeByIndex(jobj, 7), HIDDEN);
	}
	
	// Replace item names
	data->text_left->data = text_left.data();
	data->text_right->data = text_right.data();

	// Set up toggles
	change_stage(data, data->selected_stage);
}

static void apply_texture_mask(u8 *texture, const u8 *mask, int width, int height,
                               int mask_x, int mask_y, int mask_w, int mask_h)
{
	constexpr auto block_size = 64;
	constexpr auto block_width = 8;
	constexpr auto block_height = 8;
	const auto block_num_x = width / block_width;

	for (auto i = 0; i < width * height / 2; i++) {
		const auto pixel = i * 2; // 4bpp
		const auto block_index = pixel / block_size;
		const auto block_x = (block_index % block_num_x) * block_width;
		const auto block_y = (block_index / block_num_x) * block_height;
		const auto offset = pixel % block_size;
		const auto offset_x = offset % block_width;
		const auto offset_y = offset / block_width;
		const auto x = block_x + offset_x;
		const auto y = block_y + offset_y;

		// Check if in mask bounds
		if (x < mask_x || x >= mask_x + mask_w)
			continue;

		if (y < mask_y || y >= mask_y + mask_h)
			continue;

		texture[i] ^= mask[i];
	}
}

extern "C" HSD_GObj *orig_Menu_SetupItemMenu(u32 entry_point);
extern "C" HSD_GObj *hook_Menu_SetupItemMenu(u32 entry_point)
{
	// Replace item frequency toggle textures
	const auto *matanim = MenMainConIs_Top.matanim_joint->child->child->next->matanim->next;

	// Dynamically generate texture for selecting each option
	constexpr auto tex_width = align_up(384, 8);
	constexpr auto tex_height = align_up(20, 8);
	constexpr auto tex_size = tex_width * tex_height / 2;
	constexpr auto mask_width = tex_width / 6;

	const auto *base = decompress(music_stages_tex_data);
	const auto *mask = decompress(music_stages_mask_tex_data);
	
	for (auto i = 0; i < 6; i++) {
		auto *texture = new u8[tex_size];
		memcpy(texture, base, tex_size);
		apply_texture_mask(texture, mask, tex_width, tex_height,
		                   mask_width * i, 0, mask_width, tex_height);

		matanim->texanim->imagetbl[i]->img_ptr = texture;
	}

	return orig_Menu_SetupItemMenu(entry_point);
}

extern "C" bool orig_Stage_GetBGM(u32 stage_id, u32 flags, u32 *result);
extern "C" bool hook_Stage_GetBGM(u32 stage_id, u32 flags, u32 *result)
{
	int bgm_index = stage_id == Stage_BF  ? bgm_selection[5]
	              : stage_id == Stage_DL  ? bgm_selection[4]
	              : stage_id == Stage_FD  ? bgm_selection[3]
	              : stage_id == Stage_FoD ? bgm_selection[2]
	              : stage_id == Stage_PS  ? bgm_selection[1]
	              : stage_id == Stage_YS  ? bgm_selection[0]
	                                      : -1;
					      
	if (bgm_index == -1)
		return orig_Stage_GetBGM(stage_id, flags, result);
		
	*result = bgm_ids[bgm_index];
	return false;
}
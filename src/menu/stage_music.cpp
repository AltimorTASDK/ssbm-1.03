#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "hsd/mobj.h"
#include "hsd/tobj.h"
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
	u8 item_frequency;
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

template<string_literal str>
static constexpr auto item_text()
{
	return array_cat(
		text_builder::textbox<138, 138>(),
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
	item_text<"Hyrule Temple">(),
	item_text<"Icicle Mountain">(),
	item_text<"Jungle Japes">(),
	item_text<"Kongo Jungle">(),
	item_text<"Kongo Jungle N64">());

extern "C" void orig_Menu_UpdateItemDisplay(HSD_GObj *gobj, bool index_changed, bool value_changed);
extern "C" void hook_Menu_UpdateItemDisplay(HSD_GObj *gobj, bool index_changed, bool value_changed)
{
	orig_Menu_UpdateItemDisplay(gobj, index_changed, value_changed);
	
	auto *data = gobj->get<ItemMenuData>();
	auto *jobj = Menu_GetItemToggle(data, MenuSelectedIndex);

	// Hide selection indicator
	HSD_JObjSetFlagsAll(HSD_JObjGetFromTreeByIndex(jobj, 4), HIDDEN);
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
		if (x >= mask_x && x < mask_x + mask_w &&
		    y >= mask_y && y < mask_y + mask_h) {
				texture[i] ^= mask[i];
		}
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
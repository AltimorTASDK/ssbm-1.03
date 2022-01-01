#include "hsd/dobj.h"
#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "hsd/memory.h"
#include "hsd/mobj.h"
#include "hsd/tobj.h"
#include "melee/menu.h"
#include "melee/music.h"
#include "melee/preferences.h"
#include "melee/stage.h"
#include "melee/text.h"
#include "rules/saved_config.h"
#include "util/compression.h"
#include "util/math.h"
#include "util/mempool.h"
#include "util/meta.h"
#include "util/patch_list.h"
#include "util/melee/text_builder.h"
#include <cstring>
#include <gctypes.h>
#include <ogc/cache.h>

#include "resources/music/music_stages.tex.h"
#include "resources/music/music_stages_mask.tex.h"
#include "resources/rules/stage_music_header.tex.h"

constexpr auto toggle_count = 32;

struct ItemMenuData {
	u8 menu_type;
	u8 selected;
	u8 toggles[toggle_count];
	u8 selected_stage;
	u8 state;
	HSD_JObj *jobj_tree[7];
	Text *text_left;
	Text *text_right;
};

extern "C" ArchiveModel MenMainConIs_Top;
extern "C" ArchiveModel MenMainPanel_Top;

extern "C" HSD_GObj *ItemMenuGObj;

extern "C" HSD_GObj *Menu_SetupItemMenu(u32 entry_point);
extern "C" void Menu_SetupItemToggles(HSD_GObj *gobj);
extern "C" HSD_JObj *Menu_GetItemToggle(ItemMenuData *data, u8 index);
extern "C" void Menu_UpdateItemDisplay(HSD_GObj *gobj, bool index_changed, bool value_changed);
extern "C" void Menu_ItemMenuThink(HSD_GObj *gobj);
extern "C" void Menu_ItemMenuInput(HSD_GObj *gobj);

template<string_literal str, u16 scale = 138>
static constexpr auto item_text()
{
	return array_cat(
		text_builder::scale<scale, 138>(),
		text_builder::ascii<str>(),
		text_builder::reset_scale(),
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
	item_text<"Giga Bowser">(),
	item_text<"Great Bay">(),
	item_text<"Green Greens">(),
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
	item_text<"Temple">(),
	item_text<"Venom">(),
	item_text<"Yoshi's Island">(),
	item_text<"Yoshi's Island N64">());

constexpr int bgm_ids[] = {
	BGM_BalloonFight,
	BGM_BigBlue,
	BGM_Brinstar,
	BGM_BrinstarDepths,
	BGM_Corneria,
	BGM_DrMario,
	BGM_Earthbound,
	BGM_FireEmblem,
	BGM_FlatZone,
	BGM_GigaBowser,
	BGM_GreatBay,
	BGM_GreenGreens,
	BGM_IcicleMountain,
	BGM_JungleJapes,
	BGM_KongoJungle,
	BGM_KongoJungleN64,

	BGM_MachRider,
	BGM_MetalBattle,
	BGM_Mother,
	BGM_Mother2,
	BGM_MushroomKingdom,
	BGM_MushroomKingdomII,
	BGM_MuteCity,
	BGM_PokeFloats,
	BGM_PrincessPeachsCastle,
	BGM_RainbowCruise,
	BGM_SariasTheme,
	BGM_SuperMarioBros3,
	BGM_Temple,
	BGM_Venom,
	BGM_YoshisIsland,
	BGM_YoshisIslandN64
};

static mempool pool;

static const auto patches = patch_list {
	// Start cursor on BF
	// li r3, 5
	std::pair { (char*)Menu_SetupItemMenu+0x154,     0x38600005u },

	// Change item frequency index/toggle count checks from 31/32 to 32/33
	// cmplwi r27, 33
	std::pair { (char*)Menu_UpdateItemDisplay+0x38,  0x281B0021u },
	// cmplwi r0, 33
	std::pair { (char*)Menu_UpdateItemDisplay+0x64,  0x28000021u },
	// cmplwi r27, 33
	std::pair { (char*)Menu_UpdateItemDisplay+0x168, 0x281B0021u },
	// cmplwi r0, 33
	std::pair { (char*)Menu_UpdateItemDisplay+0x188, 0x28000021u },
	// subi r0, r27, 32
	std::pair { (char*)Menu_UpdateItemDisplay+0x28C, 0x381BFFE0u },
	// cmplwi r0, 33
	std::pair { (char*)Menu_UpdateItemDisplay+0x5A0, 0x28000021u },
	// cmplwi r0, 33
	std::pair { (char*)Menu_UpdateItemDisplay+0x5C0, 0x28000021u },
	// cmplwi r4, 33
	std::pair { (char*)Menu_UpdateItemDisplay+0x66C, 0x28040021u },
	// subi r0, r4, 32
	std::pair { (char*)Menu_ItemMenuThink+0x2DC,     0x3804FFE0u },
	// cmplwi r4, 33
	std::pair { (char*)Menu_ItemMenuThink+0x37C,     0x28040021u },
	// cmpwi r26, 32
	std::pair { (char*)Menu_ItemMenuThink+0x3BC,     0x2C1A0020u },
	// cmplwi r0, 32
	std::pair { (char*)Menu_SetupItemMenu+0x13C,     0x28000020u },
	// cmplwi r0, 33
	std::pair { (char*)Menu_SetupItemMenu+0x190,     0x28000021u },
	// cmpwi r27, 32
	std::pair { (char*)Menu_SetupItemMenu+0x370,     0x2C1B0020u },
	// cmplwi r27, 33
	std::pair { (char*)Menu_SetupItemMenu+0x398,     0x281B0021u },
	// cmplwi r0, 33
	std::pair { (char*)Menu_SetupItemMenu+0x6B0,     0x28000021u },
	// cmpwi r27, 32
	std::pair { (char*)Menu_SetupItemToggles+0x100,  0x2C1B0020u },
	// cmplwi r29, 33
	std::pair { (char*)Menu_SetupItemToggles+0x114,  0x281D0021u },
	// cmpwi r30, 32
	std::pair { (char*)Menu_ItemMenuInput+0x78,      0x2C1E0020u },
	// cmplwi r0, 32
	std::pair { (char*)Menu_ItemMenuInput+0xC8,      0x28000020u },
	// cmpwi r27, 32
	std::pair { (char*)Menu_ItemMenuInput+0x118,     0x2C1B0020u },
	// cmpwi r30, 32
	std::pair { (char*)Menu_ItemMenuInput+0x17C,     0x2C1E0020u },
	// cmpwi r30, 32
	std::pair { (char*)Menu_ItemMenuInput+0x1C8,     0x2C1E0020u },

	// Move data->selected_stage from 0x21 to 0x22
	// lbz r4, 0x22(r28)
	std::pair { (char*)Menu_UpdateItemDisplay+0x50,  0x889C0022u },
	// lbz r4, 0x22(r30)
	std::pair { (char*)Menu_ItemMenuThink+0x2F0,     0x889E0022u },
	// stb r0, 0x22(r30)
	std::pair { (char*)Menu_ItemMenuThink+0x3A0,     0x981E0022u },
	// lbz r3, 0x22(r25)
	std::pair { (char*)Menu_ItemMenuThink+0x3C8,     0x88790022u },
	// stb r3, 0x22(r25)
	std::pair { (char*)Menu_SetupItemMenu+0x158,     0x98790022u },
	// lbz r0, 0x22(r25)
	std::pair { (char*)Menu_SetupItemMenu+0x1A4,     0x88190022u },
	// lbz r3, 0x22(r25)
	std::pair { (char*)Menu_SetupItemMenu+0x6A4,     0x88790022u },
	// lbz r3, 0x22(r29)
	std::pair { (char*)Menu_ItemMenuInput+0x84,      0x887D0022u },
	// lbz r3, 0x22(r29)
	std::pair { (char*)Menu_ItemMenuInput+0x124,     0x887D0022u },
	// lbz r3, 0x22(r28)
	std::pair { (char*)Menu_ItemMenuInput+0x188,     0x887C0022u },
	// lbz r3, 0x22(r28)
	std::pair { (char*)Menu_ItemMenuInput+0x1D4,     0x887C0022u },
};

static int get_selected_bgm_id(ItemMenuData *data, u8 stage)
{
	const auto bgm = config.stage_bgm[stage];
	if (bgm != BGM_Undefined)
		return bgm_ids[bgm];

	return std::array {
		BGM_YoshisStory,
		BGM_PokemonStadium,
		BGM_FountainOfDreams,
		BGM_FinalDestination,
		BGM_DreamLandN64,
		BGM_Battlefield
	}[stage];
}

static void play_selected_bgm(ItemMenuData *data, u8 stage)
{
	// Restart BGM even if it's the same as the menu music
	CurrentBGMPath[0] = '\0';

	PlayBGM(get_selected_bgm_id(data, stage));
}

static void set_toggle(ItemMenuData *data, u8 index, bool toggle)
{
	data->toggles[index] = toggle;

	if (index == MenuSelectedIndex)
		MenuSelectedValue = toggle;

	auto *jobj = Menu_GetItemToggle(data, index);
	auto *on_off_switch = HSD_JObjGetFromTreeByIndex(jobj, 2);
	HSD_JObjReqAnimAll(on_off_switch, toggle ? 1.f : 0.f);
	HSD_JObjAnimAll(on_off_switch);
}

static void change_stage(ItemMenuData *data, u8 stage)
{
	// Copy selection for stage
	for (u8 i = 0; i < toggle_count; i++)
		set_toggle(data, i, i == config.stage_bgm[stage]);

	play_selected_bgm(data, stage);
}

extern "C" void orig_Menu_UpdateItemDisplay(HSD_GObj *gobj, bool index_changed, bool value_changed);
extern "C" void hook_Menu_UpdateItemDisplay(HSD_GObj *gobj, bool index_changed, bool value_changed)
{
	orig_Menu_UpdateItemDisplay(gobj, index_changed, value_changed);

	auto *data = gobj->get<ItemMenuData>();

	if (MenuSelectedIndex >= toggle_count) {
		if (value_changed)
			change_stage(data, MenuSelectedValue);
		return;
	}

	// Hide selection indicator
	auto *jobj = Menu_GetItemToggle(data, (u8)MenuSelectedIndex);
	HSD_JObjSetFlagsAll(HSD_JObjGetFromTreeByIndex(jobj, 4), HIDDEN);

	if (!value_changed)
		return;

	if (MenuSelectedValue)
		config.stage_bgm[data->selected_stage] = MenuSelectedIndex;
	else
		config.stage_bgm[data->selected_stage] = BGM_Undefined;

	for (u8 i = 0; i < toggle_count; i++) {
		// Disable all other songs
		if (i != MenuSelectedIndex)
			set_toggle(data, i, false);
	}

	play_selected_bgm(data, data->selected_stage);
}

static void apply_texture_mask(u8 *texture, const u8 *mask, int width, int height,
                               int mask_x, int mask_y, int mask_w, int mask_h)
{
	constexpr auto block_size = 64;
	constexpr auto block_width = 8;
	constexpr auto block_height = 8;
	const auto size = width * height / 2;
	const auto block_num_x = width / block_width;

	for (auto i = 0; i < size; i++) {
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

	DCStoreRange(texture, size);
}

static void pool_free(void *data)
{
	HSD_Free(data); // Default free gobj data
	pool.dec_ref();
}

static void replace_textures()
{
	if (pool.inc_ref() != 0)
		return;

	// Replace menu header name
	const auto *name = MenMainPanel_Top.matanim_joint->child->next->next->child->next->matanim;
	pool.add_texture_swap(stage_music_header_tex_data, name->texanim->imagetbl[2]);

	// Replace item frequency toggle textures
	const auto *matanim = MenMainConIs_Top.matanim_joint->child->child->next->matanim->next;

	// Dynamically generate texture for selecting each option
	constexpr auto tex_width = align_up(384, 8);
	constexpr auto tex_height = align_up(20, 8);
	constexpr auto mask_width = tex_width / 6;

	for (auto i = 0; i < 6; i++)
		pool.add_texture_swap(music_stages_tex_data, matanim->texanim->imagetbl[i]);

	const auto *mask = decompress(music_stages_mask_tex_data);

	for (auto i = 0; i < 6; i++) {
		apply_texture_mask((u8*)matanim->texanim->imagetbl[i]->img_ptr, mask,
		                   tex_width, tex_height,
		                   mask_width * i, 0, mask_width, tex_height);
	}

	delete[] mask;
}

extern "C" void orig_Menu_SetupItemToggles(HSD_GObj *gobj);
extern "C" void hook_Menu_SetupItemToggles(HSD_GObj *gobj)
{
	orig_Menu_SetupItemToggles(gobj);

	replace_textures();

	// Free assets on menu exit
	gobj->user_data_remove_func = pool_free;

	auto *data = gobj->get<ItemMenuData>();

	for (u8 i = 0; i < toggle_count; i++) {
		auto *jobj = Menu_GetItemToggle(data, i);
		// Hide item image
		HSD_JObjSetFlagsAll(HSD_JObjGetFromTreeByIndex(jobj, 7), HIDDEN);
	}

	if (!IsLanguageUS()) {
		// Load accented E
		data->text_left->sis_id = 1;
		data->text_right->sis_id = 1;
	}

	// Replace item names
	data->text_left->data = text_left.data();
	data->text_right->data = text_right.data();

	// Set up toggles
	change_stage(data, data->selected_stage);
}

extern "C" void orig_Menu_ItemMenuInput(HSD_GObj *gobj);
extern "C" void hook_Menu_ItemMenuInput(HSD_GObj *gobj)
{
	const auto buttons = Menu_GetButtonsHelper(PORT_ALL);

	if (buttons & (MenuButton_B | MenuButton_Start))
		config.save();

	orig_Menu_ItemMenuInput(gobj);

	auto *data = ItemMenuGObj->get<ItemMenuData>();

	// Allow cycling through stages with L/R
	if (buttons & MenuButton_L)
		data->selected_stage = (u8)increment_mod((int)data->selected_stage, 6);
	else if (buttons & MenuButton_R)
		data->selected_stage = (u8)decrement_mod((int)data->selected_stage, 6);
	else
		return;

	change_stage(data, data->selected_stage);

	if (MenuSelectedIndex >= toggle_count) {
		MenuSelectedValue = data->selected_stage;
		hook_Menu_UpdateItemDisplay(ItemMenuGObj, false, true);
		return;
	}

	// Force the stage selection to update
	const auto old_index = MenuSelectedIndex;
	const auto old_value = MenuSelectedValue;
	const auto old_selected = data->selected;

	MenuSelectedIndex = toggle_count;
	MenuSelectedValue = data->selected_stage;
	hook_Menu_UpdateItemDisplay(ItemMenuGObj, true, true);

	MenuSelectedIndex = old_index;
	MenuSelectedValue = old_value;
	data->selected = toggle_count;
	hook_Menu_UpdateItemDisplay(ItemMenuGObj, true, false);

	data->selected = old_selected;
}

static int get_scroll_index(ItemMenuData *data, u32 buttons)
{
	constexpr auto row_count = 16;

	// Stage rotator accessed from left or right side
	constexpr auto stage_left = toggle_count;
	constexpr auto stage_right = toggle_count + 1;

	// Toggle wrap around points
	constexpr auto top_left = 0;
	constexpr auto top_right = row_count;
	constexpr auto bottom_left = row_count - 1;
	constexpr auto bottom_right = toggle_count - 1;

	if (buttons & MenuButton_Up) {
		switch (MenuSelectedIndex) {
		case stage_left:   return bottom_left;
		case stage_right:  return bottom_right;
		case top_left:     return stage_left;
		case top_right:    return stage_right;
		default:           return MenuSelectedIndex - 1;
		}
	} else if (buttons & MenuButton_Down) {
		switch (MenuSelectedIndex) {
		case stage_left:   return top_left;
		case stage_right:  return top_right;
		case bottom_left:  return stage_left;
		case bottom_right: return stage_right;
		default:           return MenuSelectedIndex + 1;
		}
	} else if (buttons & MenuButton_Left) {
		switch (MenuSelectedIndex) {
		case stage_left:
		case stage_right:
			MenuSelectedValue = (u8)increment_mod((int)MenuSelectedValue, 6);
			return MenuSelectedIndex;
		default:
			if (MenuSelectedIndex >= row_count)
				return MenuSelectedIndex - row_count;
		}
	} else if (buttons & MenuButton_Right) {
		switch (MenuSelectedIndex) {
		case stage_left:
		case stage_right:
			MenuSelectedValue = (u8)decrement_mod((int)MenuSelectedValue, 6);
			return MenuSelectedIndex;
		default:
			if (MenuSelectedIndex < row_count)
				return MenuSelectedIndex + row_count;
		}
	}

	return MenuSelectedIndex;
}

extern "C" void orig_Menu_ItemMenuScroll(ItemMenuData *data, u32 buttons);
extern "C" void hook_Menu_ItemMenuScroll(ItemMenuData *data, u32 buttons)
{
	const auto index = (u16)get_scroll_index(data, buttons);

	if (index == MenuSelectedIndex)
		return;

	MenuSelectedIndex = index;

	if (index < toggle_count)
		MenuSelectedValue = data->toggles[index];
	else
		MenuSelectedValue = data->selected_stage;
}

extern "C" void orig_Menu_ExitToRulesMenu();
extern "C" void hook_Menu_ExitToRulesMenu()
{
	// Restore menu music
	if (MenuType == MenuType_StageMusic) {
		// Restart BGM even if it's the same as the stage music
		CurrentBGMPath[0] = '\0';

		PlayBGM(Menu_GetBGM());
	}

	orig_Menu_ExitToRulesMenu();
}

extern "C" bool orig_Stage_GetBGM(u32 stage_id, u32 flags, u32 *result);
extern "C" bool hook_Stage_GetBGM(u32 stage_id, u32 flags, u32 *result)
{
	// Restart BGM even if it's the same as the menu music
	CurrentBGMPath[0] = '\0';

	auto bgm_index = stage_id == Stage_BF  ? config.stage_bgm[5]
	               : stage_id == Stage_DL  ? config.stage_bgm[4]
	               : stage_id == Stage_FD  ? config.stage_bgm[3]
	               : stage_id == Stage_FoD ? config.stage_bgm[2]
	               : stage_id == Stage_PS  ? config.stage_bgm[1]
	               : stage_id == Stage_YS  ? config.stage_bgm[0]
	                                       : BGM_Undefined;

	if (bgm_index == BGM_Undefined)
		return orig_Stage_GetBGM(stage_id, flags, result);

	*result = bgm_ids[bgm_index];
	return false;
}
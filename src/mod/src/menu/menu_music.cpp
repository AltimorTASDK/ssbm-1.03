#include "hsd/cobj.h"
#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "hsd/memory.h"
#include "hsd/video.h"
#include "melee/menu.h"
#include "melee/music.h"
#include "melee/rules.h"
#include "melee/text.h"
#include "os/gx.h"
#include "util/compression.h"
#include "util/math.h"
#include "util/matrix.h"
#include "util/mempool.h"
#include "util/meta.h"
#include "util/patch_list.h"
#include "util/draw/render.h"
#include "util/draw/texture.h"
#include "util/melee/text_builder.h"
#include <algorithm>
#include <gctypes.h>
#include <type_traits>
#include <ogc/gx.h>

#include "resources/music/trigger_l.tex.h"
#include "resources/music/trigger_r.tex.h"

struct RandomStageMenuData {
	u8 menu_type;
	u8 selected;
	u8 toggles[30];
	HSD_JObj *jobj_tree[6];
	u8 state;
	u8 page;
	char unused03A[0x40 - 0x3A];
	Text *text[30];
};

extern "C" HSD_GObj *RandomStageMenuGObj;

extern "C" HSD_GObj *Menu_SetupRandomStageMenu(u8 state);
extern "C" void Menu_RandomStageMenuThink(HSD_GObj *gobj);
extern "C" void Menu_RandomStageMenuInput(HSD_GObj *gobj);
extern "C" void Menu_SetupRandomStageToggles(RandomStageMenuData *data);
extern "C" HSD_JObj *Menu_GetRandomStageToggle(RandomStageMenuData *data, u8 index);

template<string_literal str>
static constexpr auto stage_text()
{
	return text_builder::build(
		text_builder::kern(),
		text_builder::textbox<138, 138>(),
		text_builder::ascii<str>(),
		text_builder::end_textbox());
}

constexpr auto bgm_text = multi_array {
	stage_text<"Balloon Fight">(),
	stage_text<"Battlefield">(),
	stage_text<"Battle Theme">(),
	stage_text<"Big Blue">(),
	stage_text<"Brinstar">(),
	stage_text<"Brinstar Depths">(),
	stage_text<"Corneria">(),
	stage_text<"Dream Land N64">(),
	stage_text<"Dr. Mario">(),
	stage_text<"Earthbound">(),
	stage_text<"Final Destination">(),
	stage_text<"Fire Emblem">(),
	stage_text<"Flat Zone">(),
	stage_text<"Fountain of Dreams">(),
	stage_text<"Great Bay">(),
	stage_text<"Green Greens">(),
	stage_text<"Icicle Mountain">(),
	stage_text<"Jungle Japes">(),
	stage_text<"Kongo Jungle">(),
	stage_text<"Kongo Jungle N64">(),
	stage_text<"Mach Rider">(),
	stage_text<"Metal Battle">(),
	stage_text<"Mother">(),
	stage_text<"Mother 2">(),
	stage_text<"Multi-Man Melee 1">(),
	stage_text<"Multi-Man Melee 2">(),
	stage_text<"Mushroom Kingdom">(),
	stage_text<"Mushroom Kingdom II">(),
	stage_text<"Mute City">(),
	stage_text<u"Poké Floats">(),
	stage_text<u"Pokémon Stadium">(),
	stage_text<"Princess Peach's Castle">(),
	stage_text<"Rainbow Cruise">(),
	stage_text<"Saria's Theme">(),
	stage_text<"Super Mario Bros. 3">(),
	stage_text<"Temple">(),
	stage_text<"Venom">(),
	stage_text<"Yoshi's Island">(),
	stage_text<"Yoshi's Island N64">(),
	stage_text<"Yoshi's Story">(),

	stage_text<"All-Star Intro">(),
	stage_text<"Targets!">(),
	stage_text<"Trophy">()
};

constexpr int bgm_ids[] = {
	BGM_BalloonFight,
	BGM_Battlefield,
	BGM_BattleTheme,
	BGM_BigBlue,
	BGM_Brinstar,
	BGM_BrinstarDepths,
	BGM_Corneria,
	BGM_DreamLandN64,
	BGM_DrMario,
	BGM_Earthbound,
	BGM_FinalDestination,
	BGM_FireEmblem,
	BGM_FlatZone,
	BGM_FountainOfDreams,
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
	BGM_MultiManMelee1,
	BGM_MultiManMelee2,
	BGM_MushroomKingdom,
	BGM_MushroomKingdomII,
	BGM_MuteCity,
	BGM_PokeFloats,
	BGM_PokemonStadium,
	BGM_PrincessPeachsCastle,
	BGM_RainbowCruise,
	BGM_SariasTheme,
	BGM_SuperMarioBros3,
	BGM_Temple,
	BGM_Venom,
	BGM_YoshisIsland,
	BGM_YoshisIslandN64,
	BGM_YoshisStory,

	// Additional menu only music
	BGM_AllStarIntro,
	BGM_Targets,
	BGM_Trophy
};

constexpr auto bgm_count = std::extent_v<decltype(bgm_ids)>;
constexpr auto max_page_size = 30;
constexpr auto page_count = (bgm_count + max_page_size - 1) / max_page_size;

static mempool pool;

// L/R trigger textures
static texture texture_l;
static texture texture_r;

static int menu_bgm = -1;
static u16 selected_index[page_count] = { 0 };

static const auto patches = patch_list {
	// Expand the menu data struct to allow an extra Text pointer
	// li r3, 0xB8
	std::pair { (char*)Menu_SetupRandomStageMenu+0xB4,     0x386000B8u },
	
	// Move data->state to the formerly unused memory at +0x38 to make room for a 30th toggle
	// stb r26, 0x38(r30)
	std::pair { (char*)Menu_SetupRandomStageMenu+0x11C,    0x9B5E0038u },
	// lbz r0, 0x38(r31)
	std::pair { (char*)Menu_RandomStageMenuThink+0x2C,     0x881F0038u },
	// stb r0, 0x38(r31)
	std::pair { (char*)Menu_RandomStageMenuThink+0x6C,     0x981F0038u },
	// stb r0, 0x38(r31)
	std::pair { (char*)Menu_RandomStageMenuThink+0x78,     0x981F0038u },
	// lbz r0, 0x38(r31)
	std::pair { (char*)Menu_RandomStageMenuThink+0x7C,     0x881F0038u },
	// lbz r0, 0x38(r31)
	std::pair { (char*)Menu_RandomStageMenuThink+0xDC,     0x881F0038u },
	// lbz r0, 0x38(r31)
	std::pair { (char*)Menu_RandomStageMenuThink+0x104,    0x881F0038u },
	// lbz r0, 0x38(r31)
	std::pair { (char*)Menu_RandomStageMenuThink+0x170,    0x881F0038u },
	// stb r25, 0x38(r31)
	std::pair { (char*)Menu_RandomStageMenuThink+0x1A0,    0x9B3F0038u },
	// lbz r0, 0x38(r31)
	std::pair { (char*)Menu_RandomStageMenuThink+0x25C,    0x881F0038u },
	
	// Change toggle loops from 29 to 30
	// cmpwi r24, 0x1E
	std::pair { (char*)Menu_SetupRandomStageMenu+0x424,    0x2C18001Eu },
	// cmpwi r25, 0x1E
	std::pair { (char*)Menu_RandomStageMenuThink+0x204,    0x2C19001Eu },
	// cmpwi r25, 0x1E
	std::pair { (char*)Menu_RandomStageMenuThink+0x23C,    0x2C19001Eu },
	// cmpwi r0, 0x1E
	std::pair { (char*)Menu_RandomStageMenuInput+0x8C,     0x2800001Eu },
	// li r0, 0x1E
	std::pair { (char*)Menu_RandomStageMenuInput+0xA8,     0x3800001Eu },
	// cmpwi r29, 0x1E
	std::pair { (char*)Menu_RandomStageMenuInput+0x168,    0x2C1D001Eu },
	// cmpwi r25, 0x1E
	std::pair { (char*)Menu_SetupRandomStageToggles+0x258, 0x2C19001Eu },
	
	// Allow having all toggles disabled
	// nop
	std::pair { (char*)Menu_RandomStageMenuInput+0xD8,     0x60000000u },
};

static int get_page_size(int page)
{
	return std::min((int)bgm_count - page * max_page_size, max_page_size);
}

static void set_toggle(RandomStageMenuData *data, u8 index, bool toggle)
{
	data->toggles[index] = toggle;
	
	if (index == MenuSelectedIndex)
		MenuSelectedValue = toggle;

	auto *jobj = Menu_GetRandomStageToggle(data, index);
	auto *on_off_switch = HSD_JObjGetFromTreeByIndex(jobj, 2);
	HSD_JObjReqAnimAll(on_off_switch, toggle ? 1.f : 0.f);
	HSD_JObjAnimAll(on_off_switch);
}

static void set_page(RandomStageMenuData *data, int page)
{
	const auto page_offset = page * max_page_size;
	const auto page_size = get_page_size(page);

	data->page = (u8)page;

	// Remember selection when switching pages
	MenuSelectedIndex = selected_index[page];

	for (u8 i = 0; i < max_page_size; i++) {
		set_toggle(data, i, menu_bgm == i + page_offset);

		auto *jobj = Menu_GetRandomStageToggle(data, i);
		auto *on_off_switch = HSD_JObjGetFromTreeByIndex(jobj, 2);

		if (i < page_size) {
			data->text[i]->hidden = false;
			data->text[i]->data = bgm_text[i + page_offset];
			HSD_JObjClearFlagsAll(on_off_switch, HIDDEN);
		} else {
			data->text[i]->hidden = true;
			HSD_JObjSetFlagsAll(on_off_switch, HIDDEN);
		}
	}
}

extern "C" void orig_Menu_UpdateRandomStageDisplay(HSD_GObj *gobj,
                                                   bool index_changed, bool value_changed);
extern "C" void hook_Menu_UpdateRandomStageDisplay(HSD_GObj *gobj,
                                                   bool index_changed, bool value_changed)
{
	orig_Menu_UpdateRandomStageDisplay(gobj, index_changed, value_changed);
	
	if (!value_changed)
		return;

	auto *data = gobj->get<RandomStageMenuData>();
	
	if (MenuSelectedValue)
		menu_bgm = MenuSelectedIndex + data->page * max_page_size;
	else
		menu_bgm = -1;

	for (u8 i = 0; i < max_page_size; i++) {
		// Disable all other songs
		if (i != MenuSelectedIndex)
			set_toggle(data, i, false);
	}
	
	// Play new BGM
	PlayBGM(Menu_GetBGM());
}

extern "C" void orig_Menu_SetupRandomStageToggles(RandomStageMenuData *data);
extern "C" void hook_Menu_SetupRandomStageToggles(RandomStageMenuData *data)
{
	orig_Menu_SetupRandomStageToggles(data);
	
	// Set up toggles
	set_page(data, 0);
}

static void menu_music_gx(HSD_GObj *gobj, u32 pass)
{
	GObj_GXProcJoint(gobj, pass);
	
	if (pass != HSD_RP_BOTTOMHALF)
		return;

	const auto *data = gobj->get<RandomStageMenuData>();
	
	// Must have finished transition animation
	if (data->state != MenuState_Idle)
		return;
	
	constexpr auto color_active   = color_rgba(255, 255, 255, 255);
	constexpr auto color_inactive = color_rgba(255, 255, 255, 96);
	
	const auto &color_l = data->page != 0              ? color_active : color_inactive;
	const auto &color_r = data->page != page_count - 1 ? color_active : color_inactive;
	
	constexpr auto offset_l = vec3(-2.5f, 7.625f, 17.f);
	constexpr auto offset_r = offset_l * vec3(-1, 1, 1);
	constexpr auto size = vec2(1.f, -.5f) * 2.f;

	auto &rs = render_state::get();
	rs.reset_3d();

	rs.fill_rect(offset_l, size, color_l, texture_l,
	             uv_coord::zero, uv_coord::one, align::center);

	rs.fill_rect(offset_r, size, color_r, texture_r,
	             uv_coord::zero, uv_coord::one, align::center);
}

static void pool_free(void *data)
{
	HSD_Free(data); // Default free gobj data
	pool.dec_ref();
}

extern "C" HSD_GObj *orig_Menu_SetupRandomStageMenu(u8 state);
extern "C" HSD_GObj *hook_Menu_SetupRandomStageMenu(u8 state)
{
	auto *gobj = orig_Menu_SetupRandomStageMenu(state);
	
	// Set new GX proc
	gobj->render_cb = menu_music_gx;
	
	for (auto i = 0u; i < page_count; i++)
		selected_index[i] = 0;
	
	// Initialize new field
	auto *data = gobj->get<RandomStageMenuData>();
	data->page = 0;

	// Free assets on menu exit
	gobj->user_data_remove_func = pool_free;
	
	if (pool.inc_ref() != 0)
		return gobj;

	pool.add(init_texture(trigger_l_tex_data, &texture_l));
	pool.add(init_texture(trigger_r_tex_data, &texture_r));
	
	return gobj;
}

extern "C" void orig_Menu_RandomStageMenuInput(HSD_GObj *gobj);
extern "C" void hook_Menu_RandomStageMenuInput(HSD_GObj *gobj)
{
	const auto buttons = Menu_GetButtonsHelper(PORT_ALL);
	auto *data = RandomStageMenuGObj->get<RandomStageMenuData>();

	if (buttons & MenuButton_L) {
		set_page(data, clamp(data->page - 1, 0, (int)page_count - 1));
		Menu_PlaySFX(MenuSFX_Scroll);
	} else if (buttons & MenuButton_R) {
		set_page(data, clamp(data->page + 1, 0, (int)page_count - 1));
		Menu_PlaySFX(MenuSFX_Scroll);
	}

	orig_Menu_RandomStageMenuInput(gobj);
}

extern "C" void orig_Menu_CreateExtraRulesMenu();
extern "C" void hook_Menu_CreateExtraRulesMenu()
{
	if (MenuType != MenuType_MenuMusic)
		return orig_Menu_CreateExtraRulesMenu();

	// Exit back to the rules menu if this is menu music
	Menu_ExitToRulesMenu();
}

extern "C" void hook_Menu_RandomStageMenuScroll(RandomStageMenuData *data, u32 buttons)
{
	constexpr auto max_row_count = 15;
	int row = MenuSelectedIndex % max_row_count;
	int col = MenuSelectedIndex / max_row_count;

	if (buttons & MenuButton_Up)
		row--;
	else if (buttons & MenuButton_Down)
		row++;
	else if (buttons & MenuButton_Left)
		col--;
	else if (buttons & MenuButton_Right)
		col++;

	const auto page_size = get_page_size(data->page);
	const auto row_count = std::min(page_size, max_row_count);
	const auto col_count = (page_size + max_row_count - 1) / max_row_count;
		
	col = clamp(col, 0, col_count - 1);
	row = mod(row, row_count);

	MenuSelectedIndex = (u16)std::min(col * max_row_count + row, get_page_size(data->page) - 1);
	MenuSelectedValue = data->toggles[MenuSelectedIndex];

	selected_index[data->page] = MenuSelectedIndex;
}

extern "C" s32 orig_Menu_GetBGM();
extern "C" s32 hook_Menu_GetBGM()
{
	if (menu_bgm == -1)
		return orig_Menu_GetBGM();
		
	return bgm_ids[menu_bgm];
}
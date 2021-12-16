#include "controls/config.h"
#include "hsd/aobj.h"
#include "hsd/archive.h"
#include "hsd/dobj.h"
#include "hsd/fobj.h"
#include "hsd/jobj.h"
#include "hsd/mobj.h"
#include "hsd/pad.h"
#include "hsd/tobj.h"
#include "latency/pdf_vb.h"
#include "melee/constants.h"
#include "melee/match.h"
#include "melee/nametag.h"
#include "melee/player.h"
#include "melee/preferences.h"
#include "melee/scene.h"
#include "melee/text.h"
#include "rules/values.h"
#include "util/compression.h"
#include "util/mempool.h"
#include "util/patch_list.h"
#include "util/vector.h"
#include "util/melee/fobj_builder.h"
#include "util/melee/match.h"
#include "util/melee/pad.h"
#include "util/melee/text_builder.h"
#include <cmath>
#include <cstring>

#include "resources/css/banner_103.tex.h"
#include "resources/css/banner_103_team.tex.h"
#include "resources/css/banner_ucf.tex.h"
#include "resources/css/banner_ucf_team.tex.h"

#include "resources/css/vs_ball_crt.tex.h"
#include "resources/css/vs_ball_lcd.tex.h"
#include "resources/css/vs_ball_low.tex.h"

enum CSSPlayerState {
	CSSPlayerState_HoveringPorts = 0,
	CSSPlayerState_HoldingPuck = 1,
	CSSPlayerState_Idle = 2,
	CSSPlayerState_Unplugged = 3
};

enum CharacterIcon {
	CharacterIcon_Max = 0x19
};

struct CSSPlayerData {
	HSD_GObj *gobj;
	u8 port;
	u8 state;
	u8 held_puck;
	u16 pad008;
	u16 exit_timer;
	vec2 position;
};

struct CSSPuckData {
	u32 character;
	u8 pad004;
	u8 held_by;
	u8 pad006;
	u8 pad007;
	vec2 position;
};

struct CSSPort {
	u8 series_icon_joint;
	u8 portrait_joint;
	u8 team_joint;
	u8 closed_port_joint;
	u8 port_background_joint;
	u8 port_type_joint;
	u8 handicap_slider_joint;
	u8 cpu_level_slider_joint;
	u8 unknown_joint;
	u8 picked_character;
	u8 team;
	u8 slot_type;
	u8 last_slot_type;
	u8 color;
	u8 character_icon;
	u8 last_character_icon;
	char pad010[0x14 - 0x10];
	f32 port_bounds_x_min;
	f32 port_bounds_x_max;
	f32 team_bounds_x_min;
	f32 team_bounds_x_max;
};

extern "C" struct {
	char pad000[0x04];
	u8 *ko_stars;
	char pad008[0x10 - 0x08];
	StartMeleeData data;
} *CSSData;

extern "C" struct {
	ArchiveModel Background;
	ArchiveModel Hand;
	ArchiveModel Token;
	ArchiveModel Menu;
	ArchiveModel PressStart;
	ArchiveModel DebugCamera;
	ArchiveModel SingleMenu;
	ArchiveModel SingleOptions;
	ArchiveModel Portrait;
} *MnSlChrModels;

extern "C" u8 CSSReadyFrames;
extern "C" u8 CSSPendingSceneChange;
extern "C" u8 CSSPortCount;
extern "C" HSD_JObj *CSSMenuJObj;
extern "C" CSSPlayerData *CSSPlayers[4];
extern "C" CSSPuckData *CSSPucks[4];
extern "C" CSSPort CSSPorts[4];

extern "C" void CSS_Setup();
extern "C" bool CSS_DropPuck(u8 index);
extern "C" void CSS_UpdatePortrait(u8 port);

// All CSS toggles except rumble require 1s hold
constexpr auto TOGGLE_HOLD_TIME = 60;

constexpr auto crew_text = text_builder::build(
	text_builder::ascii<"An epic crew battle!">());

static const auto patches = patch_list {
	// Use "Survival!" voice clip for crew
	// li r3, 0x7538
	std::pair { (char*)CSS_Setup+0xF8, 0x38607538 },
};

static mempool pool;

static u8 player_states[4];
static bool is_unplugged[4];
static bool saved_is_unplugged[4];

static struct {
	u32 z_jump;
#if 0
	u32 perfect_angles;
	u32 c_up;
	u32 c_horizontal;
	u32 c_down;
	u32 tap_jump;
#endif
} toggle_timers[4];

#if 0
template<u8 a, u8 b>
constexpr auto make_color_track()
{
	constexpr auto a_f32 = (f32)a / 255.f;
	constexpr auto b_f32 = (f32)b / 255.f;

	return fobj_builder<HSD_A_FRAC_U16, 32768>::write_keys<
		HSD_A_OP_LIN,
		{ a_f32, 10 }, { a_f32, 15 }, { b_f32, 15 }, { a_f32, 15 },
		{ b_f32, 15 }, { a_f32, 15 }, { b_f32, 15 }, { a_f32, 15 }>();
}

constexpr auto color1 = color_rgb(170, 231, 255);
constexpr auto color2 = color_rgb( 97, 134, 255);
constexpr auto track_r = make_color_track<color1.r, color2.r>();
constexpr auto track_g = make_color_track<color1.g, color2.g>();
constexpr auto track_b = make_color_track<color1.b, color2.b>();
#endif

extern "C" bool check_is_cpu_puck(u8 port)
{
	// Allow anyone to move pucks of fake HMN ports
	return CSSPlayers[port]->state == CSSPlayerState_Unplugged;
}

extern "C" bool check_can_start_match(bool check_crew)
{
	auto player_count = 0;
	auto human_count = 0;

	for (auto i = 0; i < CSSPortCount; i++) {
		if (CSSPorts[i].slot_type == SlotType_None)
			continue;

		// Check that no one is holding a puck
		if (player_states[i] == CSSPlayerState_HoldingPuck)
			return false;

		// Check that no one doesn't have a valid character selected
		if (CSSPorts[i].character_icon >= CharacterIcon_Max)
			return false;

		player_count++;

		// Check if there's a real HMN port before starting a match
		if (CSSPorts[i].slot_type != SlotType_Human)
			continue;

		if (player_states[i] != CSSPlayerState_Unplugged)
			human_count++;
	}

	if (check_crew && GetGameRules()->mode == Mode_Crew && player_count > 2)
		return false;

	return human_count > 0;
}

extern "C" bool check_if_any_players()
{
	// Check if there are any players
	for (auto i = 0; i < CSSPortCount; i++) {
		if (CSSPorts[i].slot_type != SlotType_None)
			return true;
	}

	return false;
}

static void rumble_toggle(u8 port)
{
	if (!(HSD_PadCopyStatus[port].instant_buttons & Button_DPadUp))
		return;

	const auto nametag = CSSData->data.players[port].nametag;

	if (nametag == NAMETAG_MAX) {
		// Toggle port rumble setting
		const auto rumble = !GetPortRumbleFlag(port);
		SetPortRumbleFlag(port, rumble);
		if (rumble)
			HSD_PadRumble(port, 0, 0, 60);
	} else {
		// Toggle nametag rumble setting
		auto *nametag_entry = NameTag_GetEntry(nametag);
		nametag_entry->rumble = !nametag_entry->rumble;
		if (nametag_entry->rumble)
			HSD_PadRumble(port, 0, 0, 60);
	}
}

static void check_css_toggle(u8 port, u32 *timer, auto &&check_callback, auto &&apply_callback)
{
	if (!check_callback()) {
		*timer = 0;
		return;
	}

	if (++*timer < TOGGLE_HOLD_TIME)
		return;

	apply_callback();
	HSD_PadRumble(port, 0, 0, 60);
}

static void show_illegal_controls(u8 port)
{
	if (SceneMajor != Scene_VsMode)
		return;

	if (CSSPorts[port].slot_type != SlotType_Human)
		return;

	// Use red HMN indicator for illegal controls
	auto *jobj = HSD_JObjGetFromTreeByIndex(CSSMenuJObj, CSSPorts[port].port_type_joint);
	auto *mobj = jobj->u.dobj->next->mobj;
	mobj->mat->diffuse = color_rgba::hex(0xFF3333FFu);
}

static void z_jump_toggle(u8 port)
{
	auto *config = &controller_configs[port];
	const auto &pad = HSD_PadCopyStatus[port];

	check_css_toggle(port, &toggle_timers[port].z_jump,
		[&] {
			// Check if already remapped
			if (config->z_jump_bit != 0)
				return false;

			// Must be holding exactly X+Z or Y+Z
			return pad.buttons == (Button_X | Button_Z)
			    || pad.buttons == (Button_Y | Button_Z);
		},
		[&] {
			config->z_jump_bit = (u8)__builtin_ctz(pad.buttons & (Button_X | Button_Y));
		});
}

#if 0
static void perfect_angles_toggle(u8 port)
{
	auto *config = &controller_configs[port];

	check_css_toggle(port, &toggle_timers[port].perfect_angles,
		[&] {
			// Check if already using PWD
			if (config->perfect_angles)
				return false;

			// Must be holding L/R only
			const auto &pad = HSD_PadCopyStatus[port];
			constexpr auto mask = Button_L | Button_R;
			return (pad.buttons & mask) != 0 && (pad.buttons & ~mask) == 0;
		},
		[&] {
			config->perfect_angles = true;
			show_illegal_controls(port);
		});
}

static void c_up_toggle(u8 port)
{
	auto *config = &controller_configs[port];

	check_css_toggle(port, &toggle_timers[port].c_up,
		[&] {
			// Check if already using cstick utilt
			if (config->c_up == cstick_type::tilt)
				return false;

			const auto &pad = HSD_PadCopyStatus[port];
			return pad.cstick.y >= YSMASH_THRESHOLD;
		},
		[&] {
			config->c_up = cstick_type::tilt;
			show_illegal_controls(port);
		});
}

static void c_horizontal_toggle(u8 port)
{
	auto *config = &controller_configs[port];

	check_css_toggle(port, &toggle_timers[port].c_horizontal,
		[&] {
			// Check if already using cstick ftilt
			if (config->c_horizontal == cstick_type::tilt)
				return false;

			const auto &pad = HSD_PadCopyStatus[port];
			return std::abs(pad.cstick.x) >= XSMASH_THRESHOLD;
		},
		[&] {
			config->c_horizontal = cstick_type::tilt;
			show_illegal_controls(port);
		});
}

static void c_down_toggle(u8 port)
{
	auto *config = &controller_configs[port];

	check_css_toggle(port, &toggle_timers[port].c_down,
		[&] {
			// Check if already using cstick dtilt
			if (config->c_down == cstick_type::tilt)
				return false;

			const auto &pad = HSD_PadCopyStatus[port];
			return pad.cstick.y <= -YSMASH_THRESHOLD;
		},
		[&] {
			config->c_down = cstick_type::tilt;
			show_illegal_controls(port);
		});
}

static void tap_jump_toggle(u8 port)
{
	auto *config = &controller_configs[port];

	check_css_toggle(port, &toggle_timers[port].tap_jump,
		[&] {
			// Check if tap jump is already disabled
			if (!config->tap_jump)
				return false;

			// Has to be against the top of the CSS
			if (CSSPlayers[port]->position.y != 25)
				return false;

			// Require 6625 upward input
			const auto &pad = HSD_PadCopyStatus[port];
			return pad.stick.y >= YSMASH_THRESHOLD;
		},
		[&] {
			config->tap_jump = false;
			show_illegal_controls(port);
		});
}
#endif

static void reset_toggle_timers(u8 port)
{
	toggle_timers[port].z_jump = 0;
#if 0
	toggle_timers[port].perfect_angles = 0;
	toggle_timers[port].c_up = 0;
	toggle_timers[port].c_horizontal = 0;
	toggle_timers[port].c_down = 0;
	toggle_timers[port].tap_jump = 0;
#endif
}

extern "C" void orig_CSS_PlayerThink(HSD_GObj *gobj);
extern "C" void hook_CSS_PlayerThink(HSD_GObj *gobj)
{
	orig_CSS_PlayerThink(gobj);

	auto *data = gobj->get<CSSPlayerData>();

	if (CSSPendingSceneChange == 0)
		player_states[data->port] = data->state;

	// CSS toggles
	if (HSD_PadCopyStatus[data->port].err == 0) {
		rumble_toggle(data->port);
		z_jump_toggle(data->port);
#if 0
		perfect_angles_toggle(data->port);
		c_up_toggle(data->port);
		c_horizontal_toggle(data->port);
		c_down_toggle(data->port);
		tap_jump_toggle(data->port);
#endif
	} else {
		reset_toggle_timers(data->port);
	}

	if (SceneMajor != Scene_VsMode)
		return;

	// Port states get saved when entering SSS
	if (CSSPendingSceneChange == 1)
		saved_is_unplugged[data->port] = is_unplugged[data->port];

	// Player states get changed on scene change, ignore
	if (CSSPendingSceneChange != 0)
		return;

	is_unplugged[data->port] = data->state == CSSPlayerState_Unplugged;

	if (data->state != CSSPlayerState_HoldingPuck)
		return;

	// Drop fake HMN puck if a real player plugs in
	const auto puck = data->held_puck;

	if (puck >= 4 || puck == data->port)
		return;

	if (CSSPlayers[puck]->state == CSSPlayerState_Unplugged)
		return;

	if (CSSPorts[puck].slot_type != SlotType_Human)
		return;

	CSS_DropPuck(puck);
	CSS_UpdatePortrait(puck);
	data->state = CSSPlayerState_Idle;
}

extern "C" void orig_CSS_UpdatePortrait(u8 port);
extern "C" void hook_CSS_UpdatePortrait(u8 port)
{
	orig_CSS_UpdatePortrait(port);

	if (CSSPorts[port].slot_type == SlotType_Human && controller_configs[port].is_illegal())
		show_illegal_controls(port);
}

extern "C" void orig_CSS_Init(void *menu);
extern "C" void hook_CSS_Init(void *menu)
{
	// Forget unplugged state on entry from main menu
	if (SceneMinorPrevious == 0) {
		for (auto i = 0; i < 4; i++)
			is_unplugged[i] = saved_is_unplugged[i];
	}

	orig_CSS_Init(menu);

	if (!IsLanguageUS()) {
		// Always load English text for accented E
#ifdef PAL
		LoadSIS(1, "SdSlChr.ukd", "SIS_SelCharData");
#else
		LoadSIS(1, "SdSlChr.usd", "SIS_SelCharData");
#endif
	}
}

static void replace_textures()
{
	// Replace MELEE/TEAM BATTLE textures
	auto *banner = MnSlChrModels->Menu.matanim_joint->child->next->next->next->child->matanim;

	if (get_ucf_type() == ucf_type::hax) {
		pool.add_texture_swap(banner_103_tex_data,      banner->texanim->imagetbl[0]);
		pool.add_texture_swap(banner_103_team_tex_data, banner->texanim->imagetbl[1]);
	} else {
		pool.add_texture_swap(banner_ucf_tex_data,      banner->texanim->imagetbl[0]);
		pool.add_texture_swap(banner_ucf_team_tex_data, banner->texanim->imagetbl[1]);
	}

	if (is_faster_melee())
		return;

	// Replace VS text with latency mode
	auto *top_panel = MnSlChrModels->Menu.joint->child->u.dobj->mobjdesc;
	if (get_latency() == latency_mode::crt)
		pool.add_texture_swap(vs_ball_crt_tex_data, top_panel->texdesc->imagedesc);
	else if (get_latency() == latency_mode::lcd)
		pool.add_texture_swap(vs_ball_lcd_tex_data, top_panel->texdesc->imagedesc);
	else if (get_latency() == latency_mode::low)
		pool.add_texture_swap(vs_ball_low_tex_data, top_panel->texdesc->imagedesc);
}

extern "C" void orig_CSS_ChooseTopString();
extern "C" void hook_CSS_ChooseTopString()
{
#ifdef PAL
	constexpr auto index = 0x48;
#else
	constexpr auto index = 0x4A;
#endif

	if (SceneMajor == Scene_VsMode && GetGameRules()->mode == Mode_Crew)
		memcpy(SISData[0][index], crew_text.data(), crew_text.size());
	else
		orig_CSS_ChooseTopString();
}

extern "C" void orig_CSS_Setup();
extern "C" void hook_CSS_Setup()
{
	if (SceneMajor != Scene_VsMode)
		return orig_CSS_Setup();

	// Free any assets from previous CSS setup
	pool.dec_ref();
	pool.inc_ref();

#if 0
	// Replace "READY TO FIGHT" color tracks
	auto *ready_anim = MnSlChrModels->PressStart.matanim_joint->child->next->matanim;
	auto *fobj_r = ready_anim->texanim->aobjdesc->fobjdesc;
	auto *fobj_g = fobj_r->next;
	auto *fobj_b = fobj_g->next;
	fobj_r->ad = track_r.data();
	fobj_g->ad = track_g.data();
	fobj_b->ad = track_b.data();
#endif

	if (CSSData->ko_stars != nullptr) {
		if (GetGameRules()->mode == Mode_Crew) {
			// Display crew stock counts with KO stars
			for (auto i = 0; i < 6; i++)
				CSSData->ko_stars[i] = (u8)get_crew_stocks(i);
		} else {
			// Remove KO stars in non-crews
			for (auto i = 0; i < 6; i++)
				CSSData->ko_stars[i] = 0;
		}
	}

	// Force teams mode off in crews
	if (GetGameRules()->mode == Mode_Crew)
		CSSData->data.rules.is_teams = false;

	orig_CSS_Setup();

	replace_textures();

	// Don't consider fake HMN ports freshly unplugged when entering the CSS
	for (u8 i = 0; i < CSSPortCount; i++) {
		if (is_unplugged[i])
			CSSPlayers[i]->state = CSSPlayerState_Unplugged;

		player_states[CSSPlayers[i]->port] = CSSPlayers[i]->state;

		// Initialize toggle timers
		reset_toggle_timers(i);
	}
}
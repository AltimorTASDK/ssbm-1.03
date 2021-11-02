#include "hsd/aobj.h"
#include "hsd/archive.h"
#include "hsd/fobj.h"
#include "hsd/mobj.h"
#include "hsd/pad.h"
#include "hsd/tobj.h"
#include "melee/match.h"
#include "melee/player.h"
#include "melee/scene.h"
#include "menu/stage_select.h"
#include "util/vector.h"
#include "util/melee/fobj_builder.h"

enum CSSPlayerState {
	CSSPlayerState_Disabled = 0,
	CSSPlayerState_HoldingPuck = 1,
	CSSPlayerState_Idle = 2,
	CSSPlayerState_Unplugged = 3
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
	char pad000[0x09];
	u8 picked_character;
	u8 team;
	u8 slot_type;
	u8 pad00C;
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

extern "C" u8 CSSPendingSceneChange;
extern "C" u8 CSSPortCount;
extern "C" CSSPlayerData *CSSPlayers[4];
extern "C" CSSPuckData *CSSPucks[4];
extern "C" CSSPort CSSPorts[4];

extern "C" bool CSS_DropPuck(u8 index);

static bool is_unplugged[4];

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

extern "C" void orig_CSS_PlayerThink(HSD_GObj *gobj);
extern "C" void hook_CSS_PlayerThink(HSD_GObj *gobj)
{
	orig_CSS_PlayerThink(gobj);

	// Player states get changed on match start, ignore
	if (CSSPendingSceneChange != 0)
		return;
	
	auto *data = gobj->get<CSSPlayerData>();

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
	data->state = CSSPlayerState_Idle;
}

extern "C" void orig_CSS_Init(void *menu);
extern "C" void hook_CSS_Init(void *menu)
{
	// Reset original stage select flag when re-entering CSS
	use_og_stage_select = false;
	
	// Forget unplugged state on entry from main menu
	if (SceneMinorPrevious == 0) {
		for (auto i = 0; i < 4; i++)
			is_unplugged[i] = false;
	}
	
	orig_CSS_Init(menu);
}

extern "C" void orig_CSS_Setup();
extern "C" void hook_CSS_Setup()
{
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
	
	// Remove KO stars
	if (CSSData->ko_stars != nullptr) {
		for (auto i = 0; i < 6; i++)
			CSSData->ko_stars[i] = 0;
	}

	orig_CSS_Setup();

	// Don't consider fake HMN ports freshly unplugged when entering the CSS
	for (auto i = 0; i < CSSPortCount; i++) {
		if (is_unplugged[i])
			CSSPlayers[i]->state = CSSPlayerState_Unplugged;
	}
}
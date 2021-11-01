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

#include "os/os.h"

struct CSSPlayerData {
	HSD_GObj *gobj;
	u8 port;
	u8 slot_type;
	char pad006[0x0C - 0x06];
	vec2 position;
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

static u8 old_slot_type[4];

extern "C" void orig_CSS_PlayerThink(HSD_GObj *gobj);
extern "C" void hook_CSS_PlayerThink(HSD_GObj *gobj)
{
	orig_CSS_PlayerThink(gobj);

	// Slot types get changed on match start, ignore
	if (CSSPendingSceneChange)
		return;
	
	const auto *data = gobj->get<CSSPlayerData>();
	old_slot_type[data->port] = data->slot_type;
}

extern "C" void orig_CSS_Init(void *menu);
extern "C" void hook_CSS_Init(void *menu)
{
	// Reset original stage select flag when re-entering CSS
	use_og_stage_select = false;
	
	// Forget slot types on entry from main menu
	if (SceneMinorPrevious == 0) {
		for (auto i = 0; i < 4; i++)
			old_slot_type[i] = SlotType_Unspecified;
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

	for (auto i = 0; i < CSSPortCount; i++)
		CSSPlayers[i]->slot_type = old_slot_type[i];
}
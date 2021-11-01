#include "hsd/aobj.h"
#include "hsd/archive.h"
#include "hsd/fobj.h"
#include "hsd/mobj.h"
#include "hsd/tobj.h"
#include "melee/match.h"
#include "util/vector.h"
#include "util/melee/fobj_builder.h"

#include "os/os.h"

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

template<u8 a, u8 b>
constexpr auto make_fobj_track()
{
	constexpr auto a_f32 = (f32)a / 255.f;
	constexpr auto b_f32 = (f32)b / 255.f;
	
	return fobj_builder<HSD_A_FRAC_U16, 32768>::write_keys<HSD_A_OP_LIN,
		fobj_key { a_f32, 10 },
		fobj_key { a_f32, 15 },
		fobj_key { b_f32, 15 },
		fobj_key { a_f32, 15 },
		fobj_key { b_f32, 15 },
		fobj_key { a_f32, 15 },
		fobj_key { b_f32, 15 },
		fobj_key { a_f32, 15 }>();
}

constexpr auto color1 = color_rgb(170, 231, 255);
constexpr auto color2 = color_rgb( 97, 134, 255);
constexpr auto track_r = make_fobj_track<color1.r, color2.r>();
constexpr auto track_g = make_fobj_track<color1.g, color2.g>();
constexpr auto track_b = make_fobj_track<color1.b, color2.b>();

extern "C" void orig_CSS_Setup();
extern "C" void hook_CSS_Setup()
{
	// Replace "READY TO FIGHT" color tracks
	auto *ready_anim = MnSlChrModels->PressStart.matanim_joint->child->next->matanim;
	auto *fobj_r = ready_anim->texanim->aobjdesc->fobjdesc;
	auto *fobj_g = fobj_r->next;
	auto *fobj_b = fobj_g->next;
	fobj_r->ad = track_r.data();
	fobj_g->ad = track_g.data();
	fobj_b->ad = track_b.data();
	
	// Remove KO stars
	for (auto i = 0; i < 6; i++)
		CSSData->ko_stars[i] = 0;

	orig_CSS_Setup();
}
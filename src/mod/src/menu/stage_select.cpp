#include "hsd/aobj.h"
#include "hsd/archive.h"
#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "hsd/mobj.h"
#include "hsd/pad.h"
#include "hsd/robj.h"
#include "melee/match.h"
#include "melee/menu.h"
#include "melee/stage.h"
#include "rules/values.h"
#include "util/texture_swap.h"
#include "util/math.h"
#include "util/vector.h"
#include <algorithm>
#include <array>
#include <gctypes.h>
#include <ogc/cache.h>
#include <ogc/gx.h>

#include "resources/sss/random.tex.h"
#include "resources/sss/bf.tex.h"
#ifdef FULL_SSS_ROTATOR
#include "resources/sss/old/dl.tex.h"
#include "resources/sss/old/fd.tex.h"
#include "resources/sss/old/fod.tex.h"
#include "resources/sss/old/ps.tex.h"
#include "resources/sss/old/ys.tex.h"
#else
#include "resources/sss/dl.tex.h"
#include "resources/sss/fd.tex.h"
#include "resources/sss/fod.tex.h"
#include "resources/sss/ps.tex.h"
#include "resources/sss/ys.tex.h"
#endif

extern "C" u8 SelectedStageIcon;
extern "C" u8 StageLoadingState;

enum UnlockType {
	UnlockType_Hidden,
	UnlockType_Locked,
	UnlockType_Unlocked
};

enum IconID {
	Icon_YS     = 6,
	Icon_FoD    = 8,
	Icon_PS     = 18,
	Icon_BF     = 24,
	Icon_FD     = 25,
	Icon_DL     = 26,
	Icon_Random = 29,
	Icon_None   = 30,
	Icon_Max    = 30
};

struct StageSelectIcon {
	HSD_JObj *jobj;
	s32 random_cooldown;
	u8 unlocked;
	u8 anim_frame;
	u8 stage_index;
	u8 stage_id;
	vec2 select_region_size;
	vec2 select_box_scale;
};

extern "C" struct {
	u8 stage_picker;
	u8 pad001;
	u8 pad002;
	u8 force_stage_id;
	bool starting_game;
	u8 pad005;
	u8 pad006;
	u8 pad007;
	VsModeData data;
} *SSSData;

extern "C" struct {
	ArchiveModel IconLarge;
	ArchiveModel RandomIcon;
	ArchiveModel IconSpecial;
	ArchiveModel StageName;
	ArchiveModel IconDouble;
	ArchiveModel StageSelect2;
	ArchiveModel StagePreview;
	ArchiveModel SelectBox;
	ArchiveModel SelectCursor;
	ArchiveModel Position;
	ArchiveModel Background;
	ArchiveModel NowLoading;
} *MnSlMapModels;

constexpr auto ICON_SCALE = 1.f;

extern "C" StageSelectIcon StageSelectIcons[Icon_Max];

// Back up select region/select box size
static auto oss_icons = std::to_array(StageSelectIcons);

static bool is_teams()
{
	return SSSData->data.data.rules.is_teams;
}

static bool is_legal_stage(int id)
{
	switch (id) {
	case Stage_BF:
	case Stage_DL:
	case Stage_FD:
	case Stage_PS:
	case Stage_YS:
		return true;
	case Stage_FoD:
		return !is_teams();
	default:
		return false;
	}
}

static void random_icon_proc(HSD_GObj *gobj)
{
	auto *jobj = gobj->get_hsd_obj<HSD_JObj>();

	// Update matrix with constraint, then set USER_DEF_MTX so position can be adjusted
	jobj->flags &= ~USER_DEF_MTX;
	GObj_ProcAnimate(gobj);
	HSD_JObjSetupMatrix(jobj);
	jobj->flags |= USER_DEF_MTX;

	// Place random at (16.4, -9.5), opposite to the stage name
	jobj->mtx[0, 3] = jobj->position.x += 14.1f + 16.4f;
	jobj->mtx[1, 3] = jobj->position.y = -9.5f;
}

static void stage_icon_proc(HSD_GObj *gobj)
{
	auto *jobj = gobj->get_hsd_obj<HSD_JObj>();

	// Update matrix with constraint, then set USER_DEF_MTX so position can be adjusted
	jobj->flags &= ~USER_DEF_MTX;
	GObj_ProcAnimate(gobj);
	HSD_JObjSetupMatrix(jobj);
	jobj->flags |= USER_DEF_MTX;

	// Move over by half an icon width to center if FoD is removed
	if (is_teams())
		jobj->position.x += 3.3f;

	jobj->mtx[0, 3] = jobj->position.x *= ICON_SCALE;
	jobj->mtx[1, 3] = jobj->position.y = 3.8f;
}

static void setup_icon(StageSelectIcon *icon, HSD_JObj *constraint,
                       const HSD_MatAnimJoint *matanim_joint, float frame)
{
	const auto &model = MnSlMapModels->IconLarge;

	// Make new icon GObj with IconLarge jobj
	auto *gobj = GObj_Create(4, 5, 0x80);
	auto *jobj = HSD_JObjLoadDesc(model.joint);
	GObj_InitKindObj(gobj, GOBJ_KIND_JOBJ, jobj);
	GObj_SetupGXLink(gobj, GObj_GXProcJoint, GOBJ_GXLINK_MENU_TOP, 0x83);
	GObj_AddProc(gobj, stage_icon_proc, 3);

	// Set anims with given matanim_joint and frame
	HSD_JObjAddAnimAll(jobj, model.animjoint, matanim_joint, model.shapeanim_joint);
	HSD_JObjReqAnimAll(jobj, frame);
	HSD_JObjAnimAll(jobj);

	HSD_AObjWalkTree(jobj, ObjType_JObj, ObjMask_TObj, HSD_AObjStopAnim,
	                 CbType_AObj_Obj_Ptr, nullptr);

	// Constrain position to given jobj
	HSD_JObjAddConstraintPos(jobj, constraint);

	// Replace icon jobj
	HSD_JObjRemoveAll(icon->jobj);
	icon->jobj = jobj;

	// Update sizes
	jobj->scale *= ICON_SCALE;

	icon->select_region_size = { 3.10f * ICON_SCALE, 2.90f * ICON_SCALE };
	icon->select_box_scale   = { 1.05f * ICON_SCALE, 1.15f * ICON_SCALE };
}

static void setup_random_icon(HSD_JObj *random_joint)
{
	// Find the gobj with a jobj constrained to the random icon joint
	for (auto *gobj = plinkhigh_gobjs[5]; gobj != nullptr; gobj = gobj->next) {
		if (gobj->obj_kind != GOBJ_KIND_JOBJ)
			continue;

		auto *jobj = gobj->get_hsd_obj<HSD_JObj>();
		if (jobj == nullptr)
			continue;

		auto *robj = jobj->robj;
		if (robj == nullptr)
			continue;

		if ((robj->flags & TYPE_MASK) != REFTYPE_JOBJ)
			continue;

		if (robj->u.jobj == random_joint) {
			gobj->proc->callback = random_icon_proc;
			break;
		}
	}
}

static void reset_striking(bool all)
{
	// Show all legal stages
	auto visibility_changed = false;

	for (auto i = 0; i < Icon_Random; i++) {
		auto *icon = &StageSelectIcons[i];
		if (is_legal_stage(icon->stage_id) || all) {
			if (icon->unlocked != UnlockType_Unlocked) {
				HSD_JObjClearFlagsAll(icon->jobj, HIDDEN);
				icon->unlocked = UnlockType_Unlocked;
				visibility_changed = true;
			}
		} else {
			if (icon->unlocked != UnlockType_Hidden) {
				HSD_JObjSetFlagsAll(icon->jobj, HIDDEN);
				icon->unlocked = UnlockType_Hidden;
				visibility_changed = true;

				if (i == SelectedStageIcon)
					SelectedStageIcon = Icon_None;
			}
		}
	}

	if (visibility_changed)
		Menu_PlaySFX(MenuSFX_Back);
}

static void strike_stage()
{
	// Don't strike random/none
	if (SelectedStageIcon >= Icon_Random)
		return;

	// Don't allow striking the last visible legal stage
	for (auto i = 0; i <= Icon_Random; i++) {
		if (i == Icon_Random)
			return;
		if (!is_legal_stage(StageSelectIcons[i].stage_id))
			continue;
		if (i != SelectedStageIcon && StageSelectIcons[i].unlocked == UnlockType_Unlocked)
			break;
	}

	// Strike stage
	auto *icon = &StageSelectIcons[SelectedStageIcon];
	HSD_JObjSetFlagsAll(icon->jobj, HIDDEN);
	icon->unlocked = UnlockType_Hidden;

	SelectedStageIcon = Icon_None;
}

extern "C" bool orig_Stage_IsValidRandomChoice(u16 index);
extern "C" bool hook_Stage_IsValidRandomChoice(u16 index)
{
	// Only select from visible legal stages
	for (auto i = 0; i < Icon_Random; i++) {
		const auto &icon = StageSelectIcons[i];
		if (icon.stage_index != index)
			continue;
		return icon.unlocked == UnlockType_Unlocked && is_legal_stage(icon.stage_id);
	}

	return false;
}

extern "C" void orig_SSS_Think();
extern "C" void hook_SSS_Think()
{
	orig_SSS_Think();

	// Don't allow striking after picking a stage
	if (StageLoadingState != 0)
		return;

	for (auto port = 0; port < 4; port++) {
		const auto &pad = HSD_PadMasterStatus[port];

		if (pad.instant_buttons & Button_Y)
			reset_striking(should_use_oss());
		else if (pad.instant_buttons & Button_Z)
			reset_striking(false);
		else if (pad.instant_buttons & Button_X)
			strike_stage();
	}
}

#ifdef FULL_SSS_ROTATOR
static void mask_unfrozen_texture(HSD_ImageDesc *image)
{
	if (image->format != GX_TF_I4)
		return;

	constexpr auto block_width = 8;
	constexpr auto block_height = 8;
	constexpr auto block_size = block_width * block_height;
	const auto width = align_up(image->width, block_width);
	const auto height = align_up(image->height, block_height);
	const auto size = width * height / 2;
	const auto block_num_x = width / block_width;
	auto *texture = (u8*)image->img_ptr;

	for (auto i = 0; i < size; i++) {
		const auto pixel = i * 2; // 4bpp
		const auto block_index = pixel / block_size;
		const auto block_y = (block_index / block_num_x) * block_height;
		const auto offset = pixel % block_size;
		const auto offset_y = offset / block_width;
		const auto y = block_y + offset_y;

		// Black out top text
		if (y <= height / 3)
			texture[i] = 0;
	}

	DCStoreRange(texture, size);
}
#endif

static void setup_stage_names()
{
	struct tex_swap {
		s32 stage_id;
		u8 tex_id;
		u8 *texture;
	};

	constexpr tex_swap tex_swaps[] = {
		{ Stage_BF,  24, bf_tex_data },
		{ Stage_DL,  26, dl_tex_data },
		{ Stage_FD,  25, fd_tex_data },
		{ Stage_FoD, 10, fod_tex_data },
		{ Stage_PS,  14, ps_tex_data },
		{ Stage_YS,   8, ys_tex_data },
		{ -1,        29, random_tex_data }
	};

	// Change upper text for frozen stages and remove upper text if unfrozen
	auto *texanim = MnSlMapModels->StageName.matanim_joint->child->child->matanim->texanim;

	for (const auto &swap : tex_swaps) {
		unmanaged_texture_swap(swap.texture, texanim->imagetbl[swap.tex_id]);
#ifdef FULL_SSS_ROTATOR
		if (swap.stage_id != -1 && !is_stage_frozen(swap.stage_id))
			mask_unfrozen_texture(texanim->imagetbl[swap.tex_id]);
#endif
	}
}

static void hide_illegal_stages()
{
	for (auto i = 0; i < Icon_Random; i++) {
		auto *icon = &StageSelectIcons[i];
		if (!is_legal_stage(icon->stage_id)) {
			HSD_JObjSetFlagsAll(icon->jobj, HIDDEN);
			icon->unlocked = UnlockType_Hidden;
		}
	}
}

extern "C" void orig_SSS_Init(void *menu);
extern "C" void hook_SSS_Init(void *menu)
{
	if (should_use_oss()) {
		// Restore old icon select boxes for OSS
		for (auto i = 0; i < Icon_Max; i++) {
			StageSelectIcons[i].select_region_size = oss_icons[i].select_region_size;
			StageSelectIcons[i].select_box_scale   = oss_icons[i].select_box_scale;
		}
	}

	orig_SSS_Init(menu);

	if (is_widescreen()) {
		// Shift the dark overlay towards the camera so it covers the whole screen
		MnSlMapModels->NowLoading.joint->position.z += 32.f;
		MnSlMapModels->NowLoading.joint->child->position.z -= 32.f;
	}

	if (should_use_oss()) {
		// Hide non-legal stages by default on OSS
		hide_illegal_stages();
		return;
	}

	setup_stage_names();

	// Create a matanimjoint to apply the top row of IconDouble to other types of icons. This
	// has to be done because the top row's matanimjoint is the *2nd* child of the root.
	const HSD_MatAnimJoint matanim_double = {
		.child = MnSlMapModels->IconDouble.matanim_joint->child->next,
		.next = nullptr,
		.matanim = nullptr
	};

	const auto *matanim_special = MnSlMapModels->IconSpecial.matanim_joint;

	// Get stage icon position joints starting at root of Position model
	const auto *position_jobj = StageSelectIcons[Icon_BF].jobj->robj->u.jobj->parent;
	HSD_JObj *icon_joints[7];
	HSD_JObjGetTree<7>(position_jobj, icon_joints);

	setup_icon(&StageSelectIcons[Icon_BF], icon_joints[1], matanim_special, 2.f);
	setup_icon(&StageSelectIcons[Icon_DL], icon_joints[2], matanim_special, 4.f);
	setup_icon(&StageSelectIcons[Icon_FD], icon_joints[3], matanim_special, 3.f);

	if (!is_teams()) {
		setup_icon(&StageSelectIcons[Icon_FoD], icon_joints[4], &matanim_double, 7.f);
		setup_icon(&StageSelectIcons[Icon_PS],  icon_joints[5], &matanim_double, 9.f);
		setup_icon(&StageSelectIcons[Icon_YS],  icon_joints[6], &matanim_double, 6.f);
	} else {
		setup_icon(&StageSelectIcons[Icon_PS],  icon_joints[4], &matanim_double, 9.f);
		setup_icon(&StageSelectIcons[Icon_YS],  icon_joints[5], &matanim_double, 6.f);
	}

	hide_illegal_stages();

	// Replace random icon's gobj proc to apply offset
	auto *random_joint = HSD_JObjGetFromTreeByIndex(position_jobj, 17);
	setup_random_icon(random_joint);
}
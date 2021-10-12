#include "hsd/aobj.h"
#include "hsd/gobj.h"
#include "hsd/jobj.h"
#include "hsd/mobj.h"
#include "hsd/pad.h"
#include "hsd/robj.h"
#include "melee/stage.h"
#include "util/vector.h"
#include <gctypes.h>

// TODO: Not compatible with 20XX striking

extern "C" u8 SelectedStageIcon;

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
	Icon_None   = 30
};

struct StageSelectIcon {
	HSD_JObj *jobj;
	u32 pad004;
	u8 unlocked;
	u8 anim_frame;
	u8 pad00A;
	u8 stage_id;
	vec2 select_region_size;
	vec2 select_box_scale;
};

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

extern "C" StageSelectIcon StageSelectIcons[29];

static bool is_legal_stage(int id)
{
	switch (id) {
	case Stage_FoD:
	case Stage_PS:
	case Stage_YS:
	case Stage_DL:
	case Stage_BF:
	case Stage_FD:
		return true;
	default:
		return false;
	}
}

static void random_icon_proc(HSD_GObj *gobj)
{
	auto *jobj = (HSD_JObj*)gobj->hsd_obj;

	// Update matrix with constraint, then set USER_DEF_MTX so position can be adjusted
	jobj->flags &= ~USER_DEF_MTX;
	GObj_ProcAnimate(gobj);
	HSD_JObjSetupMatrix(jobj);
	jobj->flags |= USER_DEF_MTX;
	
	// Place random at (16.4, -9.5), opposite to the stage name
	jobj->mtx.get<0, 3>() = jobj->position.x += 14.1f + 16.4f;
	jobj->mtx.get<1, 3>() = jobj->position.y = -9.5f;
}

static void stage_icon_proc(HSD_GObj *gobj)
{
	auto *jobj = (HSD_JObj*)gobj->hsd_obj;

	// Update matrix with constraint, then set USER_DEF_MTX so position can be adjusted
	jobj->flags &= ~USER_DEF_MTX;
	GObj_ProcAnimate(gobj);
	HSD_JObjSetupMatrix(jobj);
	jobj->flags |= USER_DEF_MTX;
	
	jobj->mtx.get<0, 3>() = jobj->position.x *= ICON_SCALE;
	jobj->mtx.get<1, 3>() = jobj->position.y = 3.8f;
}

static void setup_icon(StageSelectIcon *icon, HSD_JObj *constraint,
                       const HSD_MatAnimJoint *matanim_joint, float frame)
{
	const auto &model = MnSlMapModels->IconLarge;

	// Make new icon GObj with IconLarge jobj
	auto *gobj = GObj_Create(4, 5, 0x80);
	auto *jobj = HSD_JObjLoadJoint(model.joint);
	GObj_InitKindObj(gobj, GOBJ_KIND_JOBJ, jobj);
	GObj_SetupGXLink(gobj, GObj_GXProcDisplay, 4, 0x83);
	GObj_CreateProcWithCallback(gobj, stage_icon_proc, 3);
	
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
			
		auto *jobj = (HSD_JObj*)gobj->hsd_obj;
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

void stage_striking(int port)
{
	const auto &pad = HSD_PadMasterStatus[port];

	if (pad.buttons & Button_Y) {
		// Show all legal stages
		for (auto i = 0; i < 29; i++) {
			auto *icon = &StageSelectIcons[i];
			if (is_legal_stage(icon->stage_id)) {
				HSD_JObjClearFlagsAll(icon->jobj, HIDDEN);
				icon->unlocked = 2;
			}
		}
	} else if (pad.buttons & Button_X) {
		// Don't strike random/none
		if (SelectedStageIcon >= Icon_Random)
			return;

		// Strike stage
		auto *icon = &StageSelectIcons[SelectedStageIcon];
		HSD_JObjSetFlagsAll(icon->jobj, HIDDEN);
		icon->unlocked = 0;
		
		SelectedStageIcon = Icon_None;
	}
}

extern "C" bool hook_Stage_IsValidRandomChoice(u16 index)
{
	return is_legal_stage(StageIndexToID[index]);
}

extern "C" void orig_SSS_Think();
extern "C" void hook_SSS_Think()
{
	orig_SSS_Think();
	
	for (auto i = 0; i < 4; i++)
		stage_striking(i);
}

extern "C" void orig_SSS_Init(void *menu);
extern "C" void hook_SSS_Init(void *menu)
{
	orig_SSS_Init(menu);
	
	// Create a matanimjoint to apply the top row of IconDouble to other types of icons. This
	// has to be done because the top row's matanimjoint is the *2nd* child of the root.
	const HSD_MatAnimJoint matanim_double_top_row = {
		.child = MnSlMapModels->IconDouble.matanim_joint->child->next,
		.next = nullptr,
		.matanim = nullptr
	};
	
	const auto *matanim_special = MnSlMapModels->IconSpecial.matanim_joint;
	
	// Get stage icon position joints starting root of Position model
	const auto *position_jobj = StageSelectIcons[Icon_BF].jobj->robj->u.jobj->parent;
	HSD_JObj *icon_joints[7];
	HSD_JObjGetTree<7>(position_jobj, icon_joints);

	setup_icon(&StageSelectIcons[Icon_BF],  icon_joints[1], matanim_special,         2.f);
	setup_icon(&StageSelectIcons[Icon_DL],  icon_joints[2], matanim_special,         4.f);
	setup_icon(&StageSelectIcons[Icon_FD],  icon_joints[3], matanim_special,         3.f);
	setup_icon(&StageSelectIcons[Icon_FoD], icon_joints[4], &matanim_double_top_row, 7.f);
	setup_icon(&StageSelectIcons[Icon_PS],  icon_joints[5], &matanim_double_top_row, 9.f);
	setup_icon(&StageSelectIcons[Icon_YS],  icon_joints[6], &matanim_double_top_row, 6.f);

	// Hide non-legal stages
	for (auto i = 0; i < 29; i++) {
		auto *icon = &StageSelectIcons[i];
		if (!is_legal_stage(icon->stage_id)) {
			HSD_JObjSetFlagsAll(icon->jobj, HIDDEN);
			icon->unlocked = 0;
		}
	}

	// Replace random icon's gobj proc to apply offset
	HSD_JObj *random_joint = HSD_JObjGetFromTreeByIndex(position_jobj, 17);
	setup_random_icon(random_joint);
}
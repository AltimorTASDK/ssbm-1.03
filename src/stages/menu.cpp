#include "hsd/aobj.h"
#include "hsd/jobj.h"
#include "hsd/mobj.h"
#include "hsd/robj.h"
#include "melee/stage.h"
#include "util/vector.h"
#include <gctypes.h>

#include "os/os.h"

enum UnlockType {
	UnlockType_Hidden,
	UnlockType_Locked,
	UnlockType_Unlocked
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

extern "C" StageSelectIcon StageSelectIcons[29];

static void setup_icon(StageSelectIcon *icon, const HSD_MatAnimJoint *matanim_joint, float frame)
{
	// Grab constraint jobj from head of jobj's robj list
	auto *constraint = icon->jobj->robj->u.jobj;

	auto *robj = icon->jobj->robj;
	while (robj != nullptr) {
		OSReport("robj flags %08X\n", robj->flags);
		robj = robj->next;
	}
	
	// Remove existing jobj
	HSD_JObjRemoveAll(icon->jobj);
	
	// Make new IconLarge with given matanim_joint
	const auto &model = MnSlMapModels->IconLarge;
	auto *jobj = HSD_JObjLoadJoint(model.joint);
	HSD_JObjAddAnimAll(jobj, model.animjoint, matanim_joint, model.shapeanim_joint);

	HSD_JObjReqAnimAll(jobj, frame);
	HSD_JObjAnimAll(jobj);
	
	HSD_AObjWalkTree(jobj, ObjType_JObj, ObjMask_TObj, HSD_AObjStopAnim,
	                 CbType_AObj_Obj_Ptr, nullptr);
			 
	// Use the constraint from the old jobj
	HSD_JObjAddConstraintPos(jobj, constraint);
			 
	// Replace icon jobj
	icon->jobj = jobj;
}

extern "C" void orig_SSS_Initialization(void *menu);
extern "C" void hook_SSS_Initialization(void *menu)
{
	orig_SSS_Initialization(menu);
	
	// Create a matanimjoint to apply the top row of IconDouble to other types of icons. This
	// has to be done because the top row's matanimjoint is the *2nd* child of the root.
	const HSD_MatAnimJoint icon_double_top_row = {
		.child = MnSlMapModels->IconDouble.matanim_joint->child->next,
		.next = nullptr,
		.matanim = nullptr
	};

	for (auto i = 0; i < 29; i++) {
		auto *icon = &StageSelectIcons[i];
		switch (icon->stage_id) {
		case Stage_BF:
			setup_icon(icon, &icon_double_top_row, 6.f);
			break;
		}
	}
}
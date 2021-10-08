#pragma once

#include "hsd/archive.h"
#include "hsd/object.h"
#include "util/vector.h"
#include "util/matrix.h"
#include "util/misc.h"
#include <gctypes.h>
#include <array>
#include <tuple>

struct HSD_AObj;
struct HSD_DObj;
struct HSD_JObjDesc;
struct HSD_RObj;
struct HSD_SList;
struct HSD_Spline;

struct HSD_AnimJoint;
struct HSD_MatAnimJoint;
struct HSD_ShapeAnimJoint;

enum JObjFlag {
	HIDDEN    = (1 << 4),
	MTX_DIRTY = (1 << 6)
};

struct HSD_JObj {
	HSD_Obj base;
	HSD_JObj *next; // Next bone with the same parent
	HSD_JObj *parent;
	HSD_JObj *child;
	u32 flags;
	union {
		struct HSD_SList *ptcl;
		struct HSD_DObj *dobj;
		struct HSD_Spline *spline;
	} u;
	vec4 rotation;
	vec3 scale;
	vec3 position;
	matrix3x4 mtx;
	vec3 *pvec;
	matrix3x4 *vmtx;
	HSD_AObj *aobj;
	HSD_RObj *robj;
	HSD_JObjDesc *desc;
};

extern "C" {
	
HSD_JObj *HSD_JObjLoadJoint(const HSD_JObjDesc *desc);

void HSD_JObjAddAnimAll(HSD_JObj *jobj, const HSD_AnimJoint *animjoint,
			const HSD_MatAnimJoint *matanim_joint,
			const HSD_ShapeAnimJoint *shapeanim_joint);
void HSD_JObjReqAnimAll(HSD_JObj *jobj, float frame);
void HSD_JObjAnimAll(HSD_JObj *jobj);

void HSD_JObjAddChild(HSD_JObj *jobj, HSD_JObj *child);
void HSD_JObjRemove(HSD_JObj *jobj);
void HSD_JObjRemoveAll(HSD_JObj *jobj);

void HSD_JObjAddConstraintPos(HSD_JObj *jobj, HSD_JObj *constraint);

void HSD_JObjSetFlagsAll(HSD_JObj *jobj, u32 flags);
void HSD_JObjClearFlagsAll(HSD_JObj *jobj, u32 flags);

void HSD_JObjGetFromTreeByIndices(const HSD_JObj *jobj, HSD_JObj **out_list,
                                  const u16 *indices, s32 count);

}

// Helper function to get single jobj
inline HSD_JObj *HSD_JObjGetFromTreeByIndex(const HSD_JObj *jobj, u16 index)
{
	HSD_JObj *result;
	HSD_JObjGetFromTreeByIndices(jobj, &result, &index, 1);
	return result;
}

// Helper function to get entire tree up to a certain size
template<size_t N>
inline void HSD_JObjGetTree(const HSD_JObj *jobj, HSD_JObj **out_list)
{
	const auto indices = for_range<N>([]<size_t ...I>() {
		return std::array { (u16)I... };
	});
	HSD_JObjGetFromTreeByIndices(jobj, out_list, indices.data(), N);
}

inline HSD_JObj *HSD_JObjFromArchiveModel(const ArchiveModel *model)
{
	auto *jobj = HSD_JObjLoadJoint(model->joint);
	HSD_JObjAddAnimAll(jobj, model->animjoint, model->matanim_joint, model->shapeanim_joint);
	return jobj;
}
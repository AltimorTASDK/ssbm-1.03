#pragma once

#include "hsd/object.h"
#include <gctypes.h>

struct HSD_FObj;
struct HSD_FObjDesc;

// What args the callback takes
enum CbType {
	// Pass aobj + user arg
	CbType_AObj,
	CbType_AObj_Float,
	CbType_AObj_Ptr,
	CbType_AObj_Int,
	// Pass aobj + containing object + user arg
	CbType_AObj_Obj,
	CbType_AObj_Obj_Float,
	CbType_AObj_Obj_Ptr,
	CbType_AObj_Obj_Int,
	// Pass aobj + containing object + containing object type + user arg
	CbType_AObj_Obj_Type,
	CbType_AObj_Obj_Type_Float,
	CbType_AObj_Obj_Type_Ptr,
	CbType_AObj_Obj_Type_Int,
};

struct HSD_AObj {
	u32 flags;
	f32 curr_frame;
	f32 rewind_frame;
	f32 end_frame;
	f32 framerate;
	HSD_FObj *fobj;
	HSD_Obj *hsd_obj;
};

struct HSD_AObjDesc {
	u32 flags;
	f32 end_frame;
	HSD_FObjDesc *fobjdesc;
	u32 obj_id;
};

extern "C" {

// Walks through AObj tree of an object containing AObjs, invoking callbacks for AObjs attached to
// objects matching obj_mask. May take a single vararg to pass to callback depending on cb_type.
void HSD_AObjWalkTree(void *obj, u32 obj_type, u32 obj_mask, auto *callback, u32 cb_type, ...);

void HSD_AObjStopAnim(HSD_AObj *aobj, void *obj, void (*callback)());

} // extern "C"
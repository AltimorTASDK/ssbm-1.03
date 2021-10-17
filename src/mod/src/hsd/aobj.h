#pragma once

#include <gctypes.h>

struct HSD_AObj;

enum ObjType {
	ObjType_DObj = 3,
	ObjType_JObj = 6,
	ObjType_MObj = 8,
	ObjType_PObj = 9,
	ObjType_RObj = 10,
	ObjType_TObj = 11
};

enum ObjMask {
	ObjMask_DObj = 0x4,
	ObjMask_JObj = 0x20,
	ObjMask_MObj = 0x80,
	ObjMask_PObj = 0x100,
	ObjMask_RObj = 0x200,
	ObjMask_TObj = 0x400
};

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

extern "C" {

// Walks through AObj tree of an object containing AObjs, invoking callbacks for AObjs attached to
// objects matching obj_mask. May take a single vararg to pass to callback depending on cb_type.
void HSD_AObjWalkTree(void *obj, u32 obj_type, u32 obj_mask, auto *callback, u32 cb_type, auto arg);

void HSD_AObjStopAnim(HSD_AObj *aobj, void *obj, void (*callback)());

}
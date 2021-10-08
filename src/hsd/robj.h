#pragma once

#include "hsd/jobj.h"
#include <gctypes.h>

enum RObjFlag {
	REFTYPE_JOBJ   = 0x10000000,
	REFTYPE_LIMIT  = 0x20000000,
	REFTYPE_IKHINT = 0x40000000,
	TYPE_MASK      = (REFTYPE_JOBJ | REFTYPE_LIMIT | REFTYPE_IKHINT)
};

struct HSD_Rvalue;

struct HSD_Exp {
	union {
		f32(*func)();
		u8 *bytecode;
	} expr;
	HSD_Rvalue *rvalue;
	u32 nb_args;
	u8 is_bytecode;
};

struct HSD_IKHint {
	f32 bone_length;
	f32 rotate_x;
};

struct HSD_RObj {
	HSD_RObj *next;
	u32 flags;
	union {
		HSD_JObj *jobj;
		HSD_Exp exp;
		f32 limit;
		HSD_IKHint ik_hint;
	} u;
	HSD_AObj *aobj;
};
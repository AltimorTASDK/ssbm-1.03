#pragma once

#include <gctypes.h>

enum GXAnimDataFormat {
	HSD_A_FRAC_FLOAT = 0,
	HSD_A_FRAC_S16   = 1,
	HSD_A_FRAC_U16   = 2,
	HSD_A_FRAC_S8    = 3,
	HSD_A_FRAC_U8    = 4
};

enum GXInterpolationType {
	HSD_A_OP_NONE = 0,
	HSD_A_OP_CON  = 1,
	HSD_A_OP_LIN  = 2,
	HSD_A_OP_SPL0 = 3,
	HSD_A_OP_SPL  = 4,
	HSD_A_OP_SLP  = 5,
	HSD_A_OP_KEY  = 6
};

struct HSD_FObjDesc {
	HSD_FObjDesc *next;
	u32 length;
	f32 startframe;
	u8 type;
	struct {
		u8 value_format : 3;
		u8 value_scale_pow : 5;
	};
	struct {
		u8 slope_format : 3;
		u8 slope_scale_pow : 5;
	};
	u8 dummy0;
	const u8 *ad;
};
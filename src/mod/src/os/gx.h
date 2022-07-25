#pragma once

#include <gctypes.h>
#include <ogc/gx.h>

// Opcodes
constexpr auto GX_NOP                 = (u8)0x00;
constexpr auto GX_DRAW_QUADS          = (u8)0x80;
constexpr auto GX_DRAW_TRIANGLES      = (u8)0x90;
constexpr auto GX_DRAW_TRIANGLE_STRIP = (u8)0x98;
constexpr auto GX_DRAW_TRIANGLE_FAN   = (u8)0xA0;
constexpr auto GX_DRAW_LINES          = (u8)0xA8;
constexpr auto GX_DRAW_LINE_STRIP     = (u8)0xB0;
constexpr auto GX_DRAW_POINTS         = (u8)0xB8;

constexpr auto GX_LOAD_BP_REG         = (u8)0x61;
constexpr auto GX_LOAD_CP_REG         = (u8)0x08;
constexpr auto GX_LOAD_XF_REG         = (u8)0x10;
constexpr auto GX_LOAD_INDX_A         = (u8)0x20;
constexpr auto GX_LOAD_INDX_B         = (u8)0x28;
constexpr auto GX_LOAD_INDX_C         = (u8)0x30;
constexpr auto GX_LOAD_INDX_D         = (u8)0x38;

constexpr auto GX_CMD_CALL_DL         = (u8)0x40;
constexpr auto GX_CMD_INVL_VC         = (u8)0x48;

constexpr auto GX_OPCODE_MASK         = (u8)0xF8;
constexpr auto GX_VAT_MASK            = (u8)0x07;

// Hardware values
enum TXMinFilter {
	TX_MIN_NEAREST                = 0, // GX_NEAR
	TX_MIN_NEAREST_MIPMAP_NEAREST = 1, // GX_NEAR_MIP_NEAR
	TX_MIN_NEAREST_MIPMAP_LINEAR  = 2, // GX_NEAR_MIP_LIN
	TX_MIN_LINEAR                 = 4, // GX_LINEAR
	TX_MIN_LINEAR_MIPMAP_NEAREST  = 5, // GX_LIN_MIP_NEAR
	TX_MIN_LINEAR_MIPMAP_LINEAR   = 6  // GX_LIN_MIP_LIN
};

struct GXTexObjInternal {
	struct {
		u32 rid : 8;
		u32 bias_clamp : 3;
		u32 max_aniso : 2;
		u32 lod_bias : 10;
		u32 diag_lod : 1;
		u32 min_filt : 3;
		u32 mag_filt : 1;
		u32 wrap_t : 2;
		u32 wrap_s : 2;
	} mode0;
	struct {
		u32 rid : 8;
		u32 : 8;
		u32 maxLOD : 8;
		u32 minLOD : 8;
	} mode1;
	struct {
		u32 rid : 8;
		u32 format : 4;
		u32 height : 10;
		u32 width : 10;
	} image0;
	struct {
		u32 rid : 8;
		u32 image_ptr : 24;
	} image3;
	u32 userData;
	u32 fmt;
	u32 tlutName;
	u16 loadCnt;
	u8 loadFmt;
	u8 flags;
};

union GXTexObjUnion {
	GXTexObj opaque;
	GXTexObjInternal internal;
};

inline union {
private:
	volatile u8  U8;
	volatile u16 U16;
	volatile u32 U32;
	volatile f32 F32;

	void write_impl(u8  value) { U8  = value; }
	void write_impl(u16 value) { U16 = value; }
	void write_impl(u32 value) { U32 = value; }
	void write_impl(f32 value) { F32 = value; }

public:
	void write(auto ...values) { (write_impl(values), ...); }
} *gx_fifo = (decltype(gx_fifo))0xCC008000;
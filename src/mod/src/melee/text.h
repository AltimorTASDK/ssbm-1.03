#pragma once

#include "util/vector.h"
#include <gctypes.h>

struct HSD_GObj;

enum class text_opcode {
	end               = 0x00,
	reset             = 0x01,
	delete_preceding  = 0x02,
	line_break        = 0x03,
	zero_width        = 0x04,
	pause             = 0x05,
	type_speed        = 0x06,
	offset            = 0x07,
	set_pointer_sub   = 0x08,
	set_pointer       = 0x09,
	spacing           = 0x0A,
	reset_spacing     = 0x0B,
	color             = 0x0C,
	clear_color       = 0x0D,
	scale             = 0x0E,
	reset_scale       = 0x0F,
	centered          = 0x10,
	reset_centered    = 0x11,
	left_aligned      = 0x12,
	reset_left_align  = 0x13,
	right_aligned     = 0x14,
	reset_right_align = 0x15,
	kerning           = 0x16,
	no_kerning        = 0x17,
	fitting           = 0x18,
	no_fitting        = 0x19,
	space             = 0x1A
};

struct Text {
	vec3 trans;
	vec2 aspect;
	char pad014[0x24 - 0x14];
	vec2 stretch;
	color_rgb background;
	u8 draw_background;
	color_rgba default_color;
	vec2 default_scale;
	vec2 default_spacing;
	char pad044[0x4D - 0x44];
	u8 hidden;
	bool pad04E;
	u8 sis_id;
	Text *next;
	HSD_GObj *gobj;
	void *callback;
	const char *data;
	char pad060[0x80 - 0x60];
	vec2 scale;
	char pad088[0xA0 - 0x88];
};

struct DevText {
	u16 x;
	u16 y;
	u8 w;
	u8 h;
	u8 cursor_x;
	u8 cursor_y;
	vec2 scale;
	u32 bg_color;
	u32 text_colors[4];
	u8 id;
	u8 line_width;
	u8 flags;
	u8 current_color;
	char *buf;
	DevText *prev;
	DevText *next;
};

struct TextKerning {
	u8 left;
	u8 right;
};

struct TextGlyphTexture {
	u8 data[512];
};

struct SIS {
#ifndef PAL
	TextKerning *kerning;
	TextGlyphTexture *textures;
#endif
	char *strings[0];
};

extern "C" {

extern Text *NameTagText;
extern SIS *SISData[5];

void LoadSIS(u32 id, const char *name, const char *symbol);

void Text_FreeAll();

int Text_CreateCanvas(u32 sis_id, HSD_GObj *camera_gobj, u8 classifier, u8 p_link, u8 p_prio,
                      u8 gx_link, u8 render_priority, u8 camera_prio);

Text *Text_Create(u32 sis_id, s32 canvas_id,
                  f32 trans_x, f32 trans_y, f32 trans_z,
                  f32 textbox_w, f32 textbox_h);
void Text_Free(Text *text);

void Text_SetFromSIS(Text *text, u32 id);
void Text_SetSubtextColor(Text *text, s32 subtext, const color_rgba &color);

DevText *DevelopText_Create(u8 id, u16 x, u16 y, s32 width, s32 height, char *buf);
void DevelopText_Remove(DevText **text);
void DevelopText_Show(HSD_GObj *unused, DevText *text);

void DevelopText_ShowBackground(DevText *text);
void DevelopText_HideBackground(DevText *text);
void DevelopText_ShowText(DevText *text);
void DevelopText_HideText(DevText *text);
void DevelopText_HideCursor(DevText *text);
void DevelopText_SetBGColor(DevText *text, const color_rgba &color);
void DevelopText_SetColorIndex(DevText *text, u8 index);
void DevelopText_SetTextColor(DevText *text, const color_rgba &color);
void DevelopText_SetScale(DevText *text, f32 x, f32 y);

void DevelopText_Erase(DevText *text);
void DevelopText_SetCursor(DevText *text, s32 x, s32 y);
void DevelopText_Printf(DevText *text, const char *fmt, ...);
void DevelopText_Print(DevText *text, const char *buf);

} // extern "C"
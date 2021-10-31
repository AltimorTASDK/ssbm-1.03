#pragma once

#include "util/vector.h"
#include <gctypes.h>

enum class text_opcode {
	end = 0x00,
	reset = 0x01,
	unknown_02 = 0x02,
	line_break = 0x03,
	unknown_04 = 0x04,
	unknown_05 = 0x05,
	unknown_06 = 0x06,
	offset = 0x07,
	unknown_08 = 0x08,
	unknown_09 = 0x09,
	scaling = 0x0A,
	reset_scaling = 0x0B,
	color = 0x0C,
	clear_color = 0x0D,
	set_textbox = 0x0E,
	reset_textbox = 0x0F,
	centered = 0x10,
	reset_centered = 0x11,
	left_aligned = 0x12,
	reset_left_align = 0x13,
	right_aligned = 0x14,
	reset_right_align = 0x15,
	kerning = 0x16,
	no_kerning = 0x17,
	fitting = 0x18,
	no_fitting = 0x19,
	space = 0x1A,
	common_character = 0x20,
	special_character = 0x40
};

struct Text {
	vec3 trans;
	vec2 aspect;
	char pad014[0x34 - 0x14];
	vec2 default_scale;
	char pad03C[0x4D - 0x3C];
	u8 hidden;
	char pad04E[0x5C - 0x4E];
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

extern "C" {
	
extern Text *NameTagText;

void Text_SetSubtextColor(Text *text, int subtext, const color_rgba &color);

}
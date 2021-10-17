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
	char pad000[0x5C];
	const char *data;
};

extern "C" {

void Text_SetSubtextColor(Text *text, int subtext, const color_rgba &color);

}
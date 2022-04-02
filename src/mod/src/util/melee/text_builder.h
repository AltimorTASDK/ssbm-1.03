#pragma once

#include "melee/text.h"
#include "util/meta.h"
#include <array>
#include <tuple>

namespace text_builder {

// Taken from PAL
#ifdef PAL
constexpr TextKerning kerning_offsets[] = {
	{13,12}, { 9, 8}, { 4, 3}, { 4, 5}, { 1, 1}, { 4, 1}, {12,13}, { 1, 1}, { 1, 1}, { 7, 6},
	{ 5, 5}, {10,11}, { 6, 6}, {10,11}, { 3, 2}, { 9, 8}, { 9,12}, { 9, 8}, { 8, 8}, { 9, 8},
	{ 9, 8}, { 9, 8}, { 9, 8}, { 9, 8}, { 9, 8}, {11,12}, {11,12}, { 5, 4}, { 6, 6}, { 2, 2},
	{ 8, 6}, { 3, 2}, { 4, 3}, { 6, 5}, { 4, 4}, { 5, 3}, { 8, 6}, { 8, 6}, { 4, 3}, { 5, 3},
	{13,11}, { 7, 6}, { 6, 4}, { 8, 6}, { 3, 1}, { 4, 3}, { 4, 3}, { 6, 4}, { 4, 2}, { 6, 4},
	{ 5, 4}, { 6, 5}, { 5, 3}, { 4, 3}, { 1, 0}, { 5, 4}, { 4, 3}, { 5, 4}, { 1, 2}, { 5, 5},
	{ 2, 1}, { 2,14}, { 5, 5}, {13,12}, { 7, 6}, { 7, 6}, { 7, 6}, { 7, 6}, { 7, 6}, { 8, 9},
	{ 7, 6}, { 8, 7}, {12,11}, { 9,12}, { 8, 6}, {12,12}, { 1, 0}, { 7, 6}, { 7, 6}, { 7, 6},
	{ 7, 6}, {10, 9}, { 8, 7}, { 8, 9}, { 7, 5}, { 7, 6}, { 2, 1}, { 7, 6}, { 6, 6}, { 7, 6},
	{ 5, 4}, { 4, 4}, { 4, 4}, { 9, 8}, { 4, 3}, { 8, 6}, { 9, 9}, { 4, 3}, { 5, 3}, { 7, 6},
	{ 7, 6}, { 9, 9}, { 7, 6}, { 8, 5}, { 4, 3}, { 8, 6}, {10, 8}, { 4, 3}, { 5, 3}, { 4, 3},
	{ 7, 6}, { 7, 6}, {10, 8}, { 7, 6}, { 8, 5}, { 6, 6}, { 4, 3}, { 8, 6}, { 9, 7}, { 4, 3},
	{ 5, 3}, { 7, 6}, { 7, 6}, { 9, 7}, { 7, 6}, { 8, 5}, { 4, 3}, { 5, 4}, { 7, 6}, { 7, 6},
	{ 4, 3}, { 8, 6}, { 9, 9}, { 4, 3}, { 5, 3}, { 4, 3}, { 7, 6}, { 7, 6}, { 9, 9}, { 7, 6},
	{ 8, 5}, { 6, 6}, { 4, 3}, { 7, 6}, { 1, 1}, { 1, 0}, { 3, 2}, { 2, 2}, { 4, 4}, { 7, 6},
	{ 6, 5}, {13,12}, { 6, 8}, { 7, 6}, { 7, 6}, { 1, 1}, { 0, 1}, { 0, 1}, { 1, 2}, { 2, 1},
};
#else
constexpr TextKerning kerning_offsets[] = {
	{ 9, 8}, { 9,12}, { 9, 8}, { 8, 8}, { 9, 8}, { 9, 8}, { 9, 8}, { 9, 8}, { 9, 8}, { 9, 8},
	{ 4, 3}, { 6, 5}, { 4, 4}, { 5, 3}, { 8, 6}, { 8, 6}, { 4, 3}, { 5, 3}, {13,11}, { 7, 6},
	{ 6, 4}, { 8, 6}, { 3, 1}, { 4, 3}, { 4, 3}, { 6, 4}, { 4, 2}, { 6, 4}, { 5, 4}, { 6, 5},
	{ 5, 3}, { 4, 3}, { 1, 0}, { 5, 4}, { 4, 3}, { 5, 4}, { 7, 6}, { 7, 6}, { 7, 6}, { 7, 6},
	{ 7, 6}, { 8, 9}, { 7, 6}, { 8, 7}, {10,10}, { 9,12}, { 8, 6}, {12,12}, { 1, 0}, { 7, 6},
	{ 7, 6}, { 7, 6}, { 7, 6}, {10, 9}, { 8, 7}, { 8, 9}, { 7, 5}, { 7, 6}, { 2, 1}, { 7, 6},
	{ 6, 6}, { 7, 6}, { 5, 5}, { 3, 2}, { 5, 5}, { 3, 2}, { 6, 6}, { 4, 4}, { 6, 5}, { 3, 2},
	{ 5, 3}, { 2, 0}, { 1, 0}, { 0, 0}, { 4, 4}, { 4, 0}, { 4, 5}, { 4, 3}, { 2, 2}, { 2, 0},
	{ 4, 4}, { 3, 0}, { 4, 3}, { 3, 0}, { 5, 3}, { 5, 3}, { 2, 2}, { 2, 0}, { 2, 2}, { 1, 0},
	{ 2, 2}, { 1, 0}, { 2, 2}, { 2, 0}, { 3, 3}, { 3, 0}, { 4, 5}, { 1, 2}, { 1, 0}, { 2, 2},
	{ 2, 0}, { 4, 4}, { 3, 0}, { 1, 1}, { 3, 2}, { 1, 0}, { 0, 0}, { 1, 1}, { 2, 1}, { 2, 0},
	{ 2, 0}, { 2, 1}, { 2, 0}, { 2, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 1, 0}, { 0, 0}, { 0, 1},
	{ 2, 1}, { 1, 0}, { 1, 0}, { 3, 3}, { 1, 1}, { 2, 1}, { 2, 2}, { 3, 2}, { 4, 4}, { 1, 1},
	{ 5, 5}, { 2, 2}, { 5, 5}, { 2, 2}, { 3, 3}, { 5, 5}, { 2, 2}, { 1, 0}, { 2, 3}, { 4, 4},
	{ 1, 1}, { 2, 3}, { 1, 1}, { 5, 4}, { 3, 1}, { 4, 6}, { 1, 3}, { 6, 5}, { 3, 2}, { 5, 4},
	{ 2, 2}, { 4, 5}, { 2, 1}, { 2, 2}, { 2, 0}, { 2, 2}, { 1, 0}, { 3, 2}, { 2, 0}, { 1, 1},
	{ 1, 0}, { 3, 4}, { 2, 0}, { 2, 2}, { 1, 0}, { 2, 1}, { 2, 1}, { 2, 1}, { 2, 0}, { 2, 2},
	{ 2, 0}, { 2, 3}, { 1, 0}, { 3, 2}, { 2, 0}, { 2, 2}, { 2, 0}, { 5, 5}, { 2, 2}, { 1, 0},
	{ 2, 2}, { 1, 0}, { 9, 3}, { 9, 2}, { 2, 2}, { 1, 1}, { 2, 2}, { 3, 2}, { 3, 6}, { 0, 0},
	{ 0, 0}, { 0, 0}, { 0, 4}, { 4, 0}, { 4, 2}, { 4, 3}, { 3, 0}, { 3, 0}, { 0, 0}, { 0, 1},
	{ 0, 0}, { 1, 1}, { 1, 1}, { 1, 1}, { 2, 2}, { 4, 4}, { 1, 1}, { 2, 3}, { 2, 2}, { 4, 4},
	{ 2, 1}, { 5, 5}, { 2, 2}, { 6, 6}, { 3, 4}, { 3, 3}, { 5, 5}, { 1, 1}, { 5, 3}, { 3, 3},
	{ 6, 6}, { 3, 3}, { 4, 3}, { 2, 2}, { 3, 0}, { 4, 5}, { 4, 0}, { 8, 8}, { 2, 1}, { 2, 1},
	{12,13}, {12,13}, {11,10}, {13,12}, {13,12}, { 7, 6}, {13,12}, {10, 8}, { 0, 0}, { 2, 1},
	{ 1, 0}, { 1, 1}, {15,15}, { 2, 1}, { 1, 1}, { 1, 0}, { 1, 1}, { 1, 0}, { 1, 1}, { 1, 0},
	{ 1, 1}, { 4, 3}, { 4, 3}, { 5, 4}, { 4, 3}, { 1, 1}, { 1, 0}, { 4, 2}, { 5, 5}, { 1, 0},
	{ 4, 3}, { 3, 2}, { 8, 6}, { 3, 2}, { 0, 0}, { 0, 1}, { 0, 0}, { 0, 0}, { 0, 1}, { 0, 0},
	{ 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 0, 0}, { 1, 1},
	{ 1, 1}, { 0, 0}, { 0, 0}, { 0, 0}, { 1, 2}, { 0, 0}, { 1, 1},
};

constexpr TextKerning kerning_offsets_special[] = {
	{ 7, 6}, { 2, 1}, { 5, 4}, { 0, 1}, { 1, 1}, { 1, 1}
};
#endif

template<u8 r, u8 g, u8 b>
constexpr auto color()
{
	return std::array { (char)text_opcode::color, (char)r, (char)g, (char)b };
};

constexpr auto end_color()
{
	return std::array { (char)text_opcode::clear_color };
};

constexpr auto kern()
{
	return std::array { (char)text_opcode::kerning };
};

constexpr auto end_kern()
{
	return std::array { (char)text_opcode::no_kerning };
};

constexpr auto left()
{
	return std::array { (char)text_opcode::left_aligned };
};

constexpr auto end_left()
{
	return std::array { (char)text_opcode::reset_left_align };
};

constexpr auto center()
{
	return std::array { (char)text_opcode::centered };
};

constexpr auto end_center()
{
	return std::array { (char)text_opcode::reset_centered };
};

constexpr auto right()
{
	return std::array { (char)text_opcode::right_aligned };
};

constexpr auto end_right()
{
	return std::array { (char)text_opcode::reset_right_align };
};

constexpr auto fit()
{
	return std::array { (char)text_opcode::fitting };
};

constexpr auto end_fit()
{
	return std::array { (char)text_opcode::no_fitting };
};

template<u16 x, u16 y>
constexpr auto scale()
{
	return std::array { (char)text_opcode::scale,
			    (char)(x >> 8), (char)(x & 0xFF),
			    (char)(y >> 8), (char)(y & 0xFF) };
};

constexpr auto reset_scale()
{
	return std::array { (char)text_opcode::reset_scale };
};

template<s16 x, s16 y>
constexpr auto spacing()
{
	return std::array { (char)text_opcode::spacing,
			    (char)(x >> 8), (char)(x & 0xFF),
			    (char)(y >> 8), (char)(y & 0xFF) };
};

constexpr auto reset_spacing()
{
	return std::array { (char)text_opcode::reset_spacing };
};

template<s16 x, s16 y>
constexpr auto offset()
{
	return std::array { (char)text_opcode::offset,
			    (char)(x >> 8), (char)(x & 0xFF),
			    (char)(y >> 8), (char)(y & 0xFF) };
};

template<u16 delay>
constexpr auto pause()
{
	return std::array { (char)text_opcode::pause, (char)(delay >> 8), (char)(delay & 0xFF) };
};

template<u16 per_char, u16 per_line>
constexpr auto type_speed()
{
	return std::array { (char)text_opcode::type_speed,
			    (char)(per_char >> 8), (char)(per_char & 0xFF),
			    (char)(per_line >> 8), (char)(per_line & 0xFF) };
};

constexpr auto br()
{
	return std::array { (char)text_opcode::line_break };
};

template<auto c>
constexpr auto char_code()
{
#ifdef PAL
	if constexpr (c == u'é')
		return '\x90';
	else
		return (char)c;
#else
	if constexpr (c >= '0' && c <= '9')
		return 0x2000 + c - '0';
	else if constexpr (c >= 'A' && c <= 'Z')
		return 0x2000 + c - 'A' + 0x0A;
	else if constexpr (c >= 'a' && c <= 'z')
		return 0x2000 + c - 'a' + 0x24;
	else if constexpr (c == ',')
		return 0x20E6;
	else if constexpr (c == '.')
		return 0x20E7;
	else if constexpr (c == ':')
		return 0x20E9;
	else if constexpr (c == ';')
		return 0x20EA;
	else if constexpr (c == '?')
		return 0x20EB;
	else if constexpr (c == '!')
		return 0x20EC;
	else if constexpr (c == '^')
		return 0x20ED;
	else if constexpr (c == '_')
		return 0x20EE;
	else if constexpr (c == u'—')
		return 0x20EF;
	else if constexpr (c == '/')
		return 0x20F0;
	else if constexpr (c == '~')
		return 0x20F1;
	else if constexpr (c == '|')
		return 0x20F2;
	else if constexpr (c == '\'')
		return 0x20F3;
	else if constexpr (c == '(')
		return 0x20F5;
	else if constexpr (c == ')')
		return 0x20F6;
	else if constexpr (c == '-')
		return 0x20FC;
	else if constexpr (c == '<')
		return 0x20FF;
	else if constexpr (c == '>')
		return 0x2100;
	else if constexpr (c == '$')
		return 0x2102;
	else if constexpr (c == u'é')
		return 0x4000;
#endif
}

template<auto c>
constexpr auto character()
{
#ifdef PAL
	return std::array { char_code<c>() };
#else
	if constexpr (c == ' ') {
		return std::array { (char)text_opcode::space };
	} else {
		constexpr auto code = char_code<c>();
		return std::array { (char)(code >> 8), (char)(code & 0xFF) };
	}
#endif
}

template<string_literal str, size_t start = 0, size_t end = decltype(str)::size - 1>
constexpr auto text()
{
	return for_range<start, end>([]<size_t ...I>() {
		return array_cat(character<str.value[I]>()...);
	});
};

template<auto c>
constexpr auto char_width()
{
	if constexpr (c == ' ') {
		return 16;
	} else {
#ifdef PAL
		constexpr auto offsets = kerning_offsets[char_code<c>() - ' ' - 1];
		return 32 - offsets.left - offsets.right + 2;
#else
		constexpr auto code = char_code<c>();
		if constexpr (code < 0x4000) {
			constexpr auto offsets = kerning_offsets[code - 0x2000];
			return 32 - offsets.left - offsets.right + 2;
		} else {
			constexpr auto offsets = kerning_offsets_special[code - 0x4000];
			return 32 - offsets.left - offsets.right + 2;
		}
#endif
	}
}

template<string_literal str, size_t start = 0, size_t end = decltype(str)::size - 1>
constexpr auto text_width()
{
	return for_range<start, end>([]<size_t ...I>() {
		return (char_width<str.value[I]>() + ...);
	});
};

struct formatting_options {
	float line_width;   // Max display width before breaking lines
	float indent;       // Horizontal offset for each line
	float line_spacing; // Extra spacing between lines
	float space_width;  // Default width of spaces
	float scale;        // Overall text scale
	bool justify;       // Set space size to pad each broken line to line_width
};

template<formatting_options options, s16 padding, string_literal str, size_t start, size_t end>
constexpr auto text_with_space_padding()
{
	constexpr auto line_spacing = (s16)(options.line_spacing * 256);
	constexpr auto pad_sequence = array_cat(spacing<padding, line_spacing>(),
	                                        character<' '>(),
	                                        spacing<0, line_spacing>());

	return for_range<start, end>([&]<size_t ...I>() {
		return array_cat([&]<auto c> {
			if constexpr (c == ' ')
				return pad_sequence;
			else
				return character<c>();
		}.template operator()<str.value[I]>()...);
	});
};

template<formatting_options options, string_literal str,
         size_t start = 0, size_t end = decltype(str)::size - 1>
constexpr auto justified_text()
{
	constexpr auto old_width = text_width<str, start, end>() * options.scale + options.indent;

	constexpr auto space_count = for_range<start, end>([]<size_t ...I>() {
		return ((str.value[I] == ' ' ? 1 : 0) + ...);
	});

	// Add spacing to each space to pad line width
	constexpr auto extra_width = options.line_width - old_width;
	constexpr auto padding = (s16)(extra_width / space_count / options.scale * 256);
	return text_with_space_padding<options, padding, str, start, end>();
};

template<formatting_options options, string_literal str,
         size_t start = 0, size_t end = decltype(str)::size - 1>
constexpr auto text_with_space_width()
{
	constexpr auto padding = (s16)((options.space_width - 16) * 256);
	return text_with_space_padding<options, padding, str, start, end>();
}

template<formatting_options options, auto line, auto space_count = 1>
constexpr auto indent()
{
	constexpr auto amount = (int)((options.indent / space_count - 16) * 256);
	if constexpr (amount > 0x7FFF || amount < -0x8000) {
		// Requires more spaces
		return indent<options, line, space_count + 1>();
	} else {
		constexpr auto fixed_scale = (u16)(options.scale * 256);
		constexpr auto line_spacing = (s16)(options.line_spacing * 256);
		return for_range<space_count>([]<size_t ...I>() {
			return array_cat(scale<256, fixed_scale>(),
			                 spacing<(s16)amount, line_spacing>(),
			                 (I, character<' '>())...,
			                 scale<fixed_scale, fixed_scale>(),
			                 spacing<0, line_spacing>(),
			                 line);
		});
	}
}

// Convert str into Melee text with line breaks inserted such that the display
// width doesn't exceed line_width
template<formatting_options options, string_literal str,
         size_t start_index = 0, size_t check_index = start_index,
         size_t break_index = start_index, float display_width = options.indent>
constexpr auto break_text()
{
	constexpr auto tail = str.value[check_index];
	constexpr auto next = str.value[check_index + 1];
	constexpr auto tail_width = [&] {
		if constexpr (tail == ' ')
			return options.space_width * options.scale;
		else
			return char_width<tail>() * options.scale;
	}();

	constexpr auto new_display_width = display_width + tail_width;

	if constexpr (new_display_width > options.line_width) {
		if constexpr (break_index == start_index) {
			// It won't fit onii-chan, so just jam it in
			constexpr auto line = text_with_space_width<options, str, start_index>();
			return indent<options, line>();
		} else {
			// Break at last good index
			constexpr auto line = [] {
				if constexpr (options.justify) {
					return justified_text<
						options, str, start_index, break_index>();
				} else {
					return text_with_space_width<
						options, str, start_index, break_index>();
				}
			}();
			if constexpr (str.value[break_index] == ' ') {
				// Skip space
				return array_cat(indent<options, line>(), br(),
				                 break_text<options, str, break_index + 1>());
			} else {
				return array_cat(indent<options, line>(), br(),
				                 break_text<options, str, break_index>());
			}
		}
	} else if constexpr (next == '\0') {
		// Consumed whole string
		constexpr auto line = text_with_space_width<options, str, start_index>();
		return indent<options, line>();
	} else if constexpr (next == ' ' || tail == '-') {
		// Before whitespace/after hyphen, update break index and check next index
		return break_text<options, str, start_index, check_index + 1,
		                  check_index + 1, new_display_width>();
	} else {
		// Can't break here, preserve break index and check next index
		return break_text<options, str, start_index, check_index + 1,
		                  break_index, new_display_width>();
	}
};

// Format text according to options
template<formatting_options options, string_literal str>
constexpr auto format_text()
{
	constexpr auto fixed_scale = (u16)(options.scale * 256);
	constexpr auto line_spacing = (s16)(options.line_spacing * 256);
	if constexpr (options.line_width != 0) {
		return array_cat(scale<fixed_scale, fixed_scale>(),
		                 spacing<0, line_spacing>(),
		                 break_text<options, str>());
	} else {
		return array_cat(scale<fixed_scale, fixed_scale>(),
		                 spacing<0, line_spacing>(),
		                 indent<options, text_with_space_width<options, str>()>());
	}
};

constexpr auto build(auto &&...components)
{
	return array_cat(components..., std::array { (char)text_opcode::end });
}

}
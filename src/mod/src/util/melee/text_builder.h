#pragma once

#include "melee/text.h"
#include "util/meta.h"
#include <array>
#include <tuple>

namespace text_builder {

// Taken from PAL
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
constexpr auto character_pal()
{
	if constexpr (c == u'é')
		return '\x90';
	else
		return (char)c;
}

template<auto c>
constexpr auto character()
{
#ifdef PAL
	return std::array { character_pal<c>() };
#else
	if constexpr (c >= '0' && c <= '9')
		return std::array { '\x20', (char)(c - '0') };
	else if constexpr (c >= 'A' && c <= 'Z')
		return std::array { '\x20', (char)(c - 'A' + 0x0A) };
	else if constexpr (c >= 'a' && c <= 'z')
		return std::array { '\x20', (char)(c - 'a' + 0x24) };
	else if constexpr (c == ',')
		return std::array { '\x20', '\xE6' };
	else if constexpr (c == '.')
		return std::array { '\x20', '\xE7' };
	else if constexpr (c == ':')
		return std::array { '\x20', '\xE9' };
	else if constexpr (c == '?')
		return std::array { '\x20', '\xEB' };
	else if constexpr (c == '!')
		return std::array { '\x20', '\xEC' };
	else if constexpr (c == '/')
		return std::array { '\x20', '\xF0' };
	else if constexpr (c == '\'')
		return std::array { '\x20', '\xF3' };
	else if constexpr (c == '(')
		return std::array { '\x20', '\xF5' };
	else if constexpr (c == ')')
		return std::array { '\x20', '\xF6' };
	else if constexpr (c == '-')
		return std::array { '\x20', '\xFC' };
	else if constexpr (c == u'é')
		return std::array { '\x40', '\x00' };
	else if constexpr (c == ' ')
		return std::array { (char)text_opcode::space };
#endif
}

template<string_literal str>
constexpr auto text()
{
	return for_range<decltype(str)::size - 1>([]<size_t ...I>() {
		return array_cat(character<str.value[I]>()...);
	});
};

template<auto c>
constexpr auto char_width()
{
	if constexpr (c == ' ') {
		return 16;
	} else {
		constexpr auto index = character_pal<c>() - ' ' - 1;
		return 32 - kerning_offsets[index].left - kerning_offsets[index].right + 2;
	}
}

template<string_literal str>
constexpr auto text_width()
{
	return for_range<decltype(str)::size - 1>([]<size_t ...I>() {
		return (char_width<str.value[I]>() + ...);
	});
};

template<auto line_width, int check_size, string_literal str>
constexpr auto split_text_impl()
{
	constexpr auto tail = str.value[check_size - 1];
	constexpr auto next = str.value[check_size];
	constexpr auto sub = str.template substring<0, check_size>();
	constexpr auto str_size = decltype(str)::size - 1;

	// Only break before whitespace or after hyphen
	constexpr auto can_break = next != '\0' && next != ' ' && tail != '-';

	if constexpr (can_break && text_width<sub>() <= line_width) {
		if constexpr (check_size == str_size) {
			// Consumed whole string
			return std::make_tuple(sub);
		} else {
			constexpr auto remainder = str.template substring<check_size, str_size>();
			return tuple_cat(std::make_tuple(sub),
			                 split_text_impl<line_width, decltype(remainder)::size - 1,
			                                 remainder>());
		}
	} else if constexpr (check_size == 1) {
		// It won't fit onii-chan, so just jam it in
		return std::make_tuple(str);
	} else {
		// Try reducing the line size
		return split_text_impl<line_width, check_size - 1, str>();
	}
};

// Break str into a tuple of lines with a display width <= line_width
template<auto line_width, string_literal str>
constexpr auto split_text()
{
	return split_text_impl<line_width, decltype(str)::size - 1, str>();
};

// Convert str into Melee text with line breaks inserted such that the display
// width doesn't exceed line_width
template<auto line_width, string_literal str>
constexpr auto break_lines()
{
	constexpr auto tuple = split_text<line_width, str>();

	return for_range<sizeof_tuple<decltype(tuple)>>([&]<size_t ...I>() {
		return array_cat(array_cat(text<std::get<I>(tuple)>(), br())...);
	});
};

constexpr auto build(auto &&...components)
{
	return array_cat(components..., std::array { (char)text_opcode::end });
}

}
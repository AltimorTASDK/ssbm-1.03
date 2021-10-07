
#pragma once

#include "melee/text.h"
#include "util/misc.h"
#include <tuple>

namespace text_builder {

template<char c>
constexpr auto ascii_to_melee()
{
	if constexpr (c >= '0' && c <= '9')
		return std::make_tuple('\x20', c);
	else if constexpr (c >= 'A' && c <= 'Z')
		return std::make_tuple('\x20', (char)(c - 'A' + 0x0A));
	else if constexpr (c >= 'a' && c <= 'z')
		return std::make_tuple('\x20', (char)(c - 'a' + 0x24));
	else if constexpr (c == ',')
		return std::make_tuple('\x20', '\xE6');
	else if constexpr (c == '.')
		return std::make_tuple('\x20', '\xE7');
	else if constexpr (c == '\'')
		return std::make_tuple('\x20', '\xF3');
	else if constexpr (c == '-')
		return std::make_tuple('\x20', '\xFC');
	else if constexpr (c == ' ')
		return std::make_tuple((char)text_opcode::space);
}

template<string_literal str>
constexpr auto ascii()
{
	return for_range<decltype(str)::size - 1>([]<size_t ...I>() {
		return std::tuple_cat(ascii_to_melee<str.value[I]>()...);
	});
};

template<u8 r, u8 g, u8 b>
constexpr auto color()
{
	return std::make_tuple((char)text_opcode::color, (char)r, (char)g, (char)b);
};

constexpr auto end_color()
{
	return std::make_tuple((char)text_opcode::clear_color);
};

constexpr auto kern()
{
	return std::make_tuple((char)text_opcode::kerning);
};

constexpr auto left()
{
	return std::make_tuple((char)text_opcode::left_aligned);
};

constexpr auto end_left()
{
	return std::make_tuple((char)text_opcode::reset_left_align);
};

constexpr auto fit()
{
	return std::make_tuple((char)text_opcode::fitting);
};

constexpr auto end_fit()
{
	return std::make_tuple((char)text_opcode::no_fitting);
};

template<u16 x, u16 y>
constexpr auto textbox()
{
	return std::make_tuple((char)text_opcode::set_textbox,
			       (char)(x >> 16), (char)(x & 0xFF),
			       (char)(y >> 16), (char)(y & 0xFF));
};

constexpr auto end_textbox()
{
	return std::make_tuple((char)text_opcode::reset_textbox);
};

template<u16 a, u16 b>
constexpr auto unk06()
{
	return std::make_tuple((char)text_opcode::unknown_06,
			       (char)(a >> 16), (char)(a & 0xFF),
			       (char)(b >> 16), (char)(b & 0xFF));
};

constexpr auto br()
{
	return std::make_tuple((char)text_opcode::line_break);
};

constexpr auto build(auto &&...components)
{
	return std::apply([](auto ...chars) {
		return std::array { chars..., (char)text_opcode::end };
	}, std::tuple_cat(components...));
}

}
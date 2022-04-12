#pragma once

#include "melee/text.h"
#include "util/meta.h"
#include <array>

namespace text_builder {

template<auto c>
constexpr auto character()
{
#ifdef PAL
	if constexpr (c == u'é')
		return std::array { '\x90' };
	else
		return std::array { (char)c };
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
	return for_range<str.size>([]<size_t ...I>() {
		return array_cat(character<str.value[I]>()...);
	});
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

constexpr auto build(auto &&...components)
{
	return array_cat(components..., std::array { (char)text_opcode::end });
}

} // namespace text_builder
#pragma once

#include "hsd/pad.h"
#include "melee/player.h"
#include "util/vector.h"
#include <cmath>
#include <gctypes.h>

// 0.2750
constexpr auto DEADZONE = 22;

// 1.0
constexpr auto STICK_MAX = 80;

namespace detail {
constexpr auto qnum = 5;

inline const PADStatus &get_input_impl(int port, int offset)
{
	// index is guaranteed to be in range [0, 8], avoid modulo
	auto index = HSD_PadLibData.qread + offset;
	if (index >= qnum)
		index -= qnum;

	return HSD_PadLibData.queue[index].stat[port];
}
}

template<int offset>
inline const PADStatus &get_input(int port)
{
	constexpr auto real_offset = (((offset - 1) % detail::qnum) + detail::qnum) % detail::qnum;
	return detail::get_input_impl(port, real_offset);
}

inline bool check_ucf_xsmash(const Player *player)
{
	const auto &prev_input = get_input<-2>(player->port);
	const auto &current_input = get_input<0>(player->port);
	const auto delta = current_input.stick.x - prev_input.stick.x;
	return delta * delta > 75 * 75;
}

inline int abs_coord_to_int(float x)
{
	// Small bias converts Nana coords back to the corresponding Popo coords
	return static_cast<int>(std::abs(x) * 80 - .0001f) + 1;
}

inline bool is_rim_coord(const vec2 &coords)
{
	const auto converted = vec2i(abs_coord_to_int(coords.x) + 1,
	                             abs_coord_to_int(coords.y) + 1);
	return converted.length_sqr() > 80 * 80;
}

inline s8 popo_to_nana(float x)
{
	return x >= 0 ? (s8)(x * 127) : (s8)(x * 128);
}

inline vec2c popo_to_nana(const vec2 &coords)
{
	return coords.map<vec2c>([](auto x) { return popo_to_nana(x); });
}
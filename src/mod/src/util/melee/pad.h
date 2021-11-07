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

// 0.3
constexpr auto TRIGGER_DEADZONE = 42;

// 1.0
constexpr auto TRIGGER_MAX = 140;

namespace detail {
const PADStatus &get_input_impl(int port, int offset);
const PADStatus &get_nana_input_impl(const Player *nana, int offset);
}

template<int offset>
inline const PADStatus &get_input(int port)
{
	constexpr auto real_offset = (((offset - 1) % PAD_QNUM) + PAD_QNUM) % PAD_QNUM;
	return detail::get_input_impl(port, real_offset);
}

template<int offset>
inline const PADStatus &get_nana_input(const Player *nana)
{
	constexpr auto real_offset = ((offset % NANA_BUFFER) + NANA_BUFFER) % NANA_BUFFER;
	return detail::get_nana_input_impl(nana, real_offset);
}

template<int offset>
inline const PADStatus &get_character_input(const Player *player)
{
	if (!player->is_backup_climber)
		return get_input<offset>(player->port);
	else
		return get_nana_input<offset>(player);
}

inline bool check_ucf_xsmash(const Player *player)
{
	const auto &prev_input = get_character_input<-2>(player);
	const auto &current_input = get_character_input<0>(player);
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
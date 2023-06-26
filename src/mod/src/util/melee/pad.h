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
constexpr auto LR_DEADZONE = 42;

// 1.0
constexpr auto LR_MAX = 140;

constexpr auto XSMASH_THRESHOLD = .8000f;

constexpr auto YSMASH_THRESHOLD = .6625f;

namespace detail {

constexpr auto PAD_BUFFER_SIZE = 4;
constexpr auto PAD_BUFFER_MASK = PAD_BUFFER_SIZE - 1;

struct alignas(4) port_pad_buffer {
	PADStatus entries[PAD_BUFFER_SIZE];
	u8 index;
};

inline port_pad_buffer pad_buffer[6];

const PADStatus &get_input_impl(int port, int offset);
const PADStatus &get_buffer_input_impl(const Player *player, int offset);
const PADStatus &get_nana_input_impl(const Player *nana, int offset);

} // namespace detail

template<int offset>
inline const PADStatus &get_input(int port)
{
	constexpr auto real_offset = (((offset - 1) % PAD_QNUM) + PAD_QNUM) % PAD_QNUM;
	return detail::get_input_impl(port, real_offset);
}

template<int offset>
inline const PADStatus &get_nana_input(const Player *nana)
{
	constexpr auto real_offset = (((offset - 1) % NANA_BUFFER) + NANA_BUFFER) % NANA_BUFFER;
	return detail::get_nana_input_impl(nana, real_offset);
}

template<int offset>
inline const PADStatus &get_pad_buffer(const Player *player)
{
	const auto &buffer = detail::pad_buffer[player->slot];
	if constexpr (offset == 0)
		return buffer.entries[buffer.index];
	else
		return buffer.entries[(buffer.index + offset) & detail::PAD_BUFFER_MASK];
}

template<int offset>
inline const PADStatus &get_character_input(const Player *player)
{
	if (player->character_id == CID_Nana)
		return get_nana_input<offset>(player);
	else
		return get_pad_buffer<offset>(player);
}

inline bool check_ucf_xsmash(const Player *player)
{
	// Designed by tauKhan
	const auto &prev_input = get_character_input<-2>(player);
	const auto &current_input = get_character_input<0>(player);
	const auto delta = current_input.stick.x - prev_input.stick.x;
	return delta * delta > 75 * 75;
}

constexpr int abs_coord_to_int(float x)
{
	// Small bias converts Nana coords back to the corresponding Popo coords
	return static_cast<int>(std::abs(x) * 80 - 0.0001f) + 1;
}

inline vec2 convert_hw_coords(const vec2b &hw)
{
	// Convert hardware stick coordinates to what Melee uses
	auto hw_signed = vec2c(hw - vec2b(128, 128));

	HSD_PadClamp(&hw_signed.x, &hw_signed.y, HSD_PadLibData.clamp_stickShift,
				                 HSD_PadLibData.clamp_stickMin,
				                 HSD_PadLibData.clamp_stickMax);

	return hw_signed.map<vec2>([](auto x) {
		return std::abs(x) > DEADZONE ? (float)x / HSD_PadLibData.scale_stick : 0.f;
	});
}

constexpr bool is_rim_coord(const vec2 &coords)
{
	const auto converted = vec2i(abs_coord_to_int(coords.x) + 1,
	                             abs_coord_to_int(coords.y) + 1);
	return converted.length_sqr() > 80 * 80;
}

constexpr s8 popo_to_nana(float x)
{
	return x >= 0 ? (s8)(x * 127) : (s8)(x * 128);
}

constexpr float popo_to_nana_float(float x)
{
	return x >= 0 ? (float)((s8)(x * 127)) / 127 : (float)((s8)(x * 128)) / 128;
}

constexpr vec2c popo_to_nana(const vec2 &coords)
{
	return coords.map<vec2c>([](auto x) { return popo_to_nana(x); });
}

constexpr float get_stick_angle(const vec2 &coords)
{
	return std::atan2(coords.y, coords.x);
}

constexpr float get_stick_angle_abs(const vec2 &coords)
{
	return std::atan2(std::abs(coords.y), std::abs(coords.x));
}

constexpr float get_stick_angle_abs_x(const vec2 &coords)
{
	return std::atan2(coords.y, std::abs(coords.x));
}
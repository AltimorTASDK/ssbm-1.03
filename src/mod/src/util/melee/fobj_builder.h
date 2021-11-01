#pragma once

#include "hsd/fobj.h"
#include "util/meta.h"
#include <gctypes.h>
#include <numeric>

struct fobj_key {
	f32 value;
	s32 time;
};

template<u8 format, u32 scale>
class fobj_builder {
	template<auto value>
	static constexpr auto pack()
	{
		constexpr auto low = value & 0x7F;

		if constexpr (value >= 128)
			return array_cat(std::array { (u8)(low | 0x80) }, pack<(value >> 7)>());
		else
			return std::array { (u8)low };
	}

	template<auto byte_count, auto value>
	static constexpr auto write_int()
	{
		return for_range<byte_count>([&]<size_t ...I>() {
			return std::array { (u8)(value >> (I * 8))... };
		});
	}

	template<f32 value>
	static constexpr auto write_float()
	{
		if constexpr (format == HSD_A_FRAC_FLOAT) {
			constexpr auto bits = std::bit_cast<u32, f32>(value);
			return write_int<4, bits>();
		} else {
			constexpr auto scaled = (s32)std::round(value * scale);
			if constexpr (format == HSD_A_FRAC_S16 || format == HSD_A_FRAC_U16)
				return write_int<2, scaled>();
			else if constexpr (format == HSD_A_FRAC_S8 || format == HSD_A_FRAC_U8)
				return write_int<1, scaled>();
		}
		
	}

	template<fobj_key key>
	static constexpr auto write_key()
	{
		static_assert(format == HSD_A_OP_CON || format == HSD_A_OP_LIN);
		return array_cat(write_float<key.value>(), pack<key.time>());
	}
	
public:
	template<u8 interpolation, fobj_key ...keys>
	static constexpr auto write_keys()
	{
		return array_cat(
			pack<interpolation | ((sizeof...(keys) - 1) << 4)>(),
			write_key<keys>()...);
	}
};
#pragma once

#include <cstddef>
#include <gctypes.h>

template<size_t N>
class bitmask {
	u32 words[(N + 31) / 32];

public:
	bool get(size_t bit)
	{
		return words[bit / 32] & (1 << (bit % 32));
	}

	void set(size_t bit, bool value)
	{
		if (value)
			words[bit / 32] |= 1 << (bit % 32);
		else
			words[bit / 32] &= ~(1 << (bit % 32));
	}
};

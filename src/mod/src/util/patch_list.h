#pragma once

#include <cstddef>
#include <ogc/cache.h>
#include <tuple>

#ifdef DOL
// Store patches to be applied by patch_dol.py
#define PATCH_LIST(...) \
	[[gnu::section(".patches"), gnu::used]] \
	static constinit auto patches##__COUNTER__ = patch_list(__VA_ARGS__)

template<typename T, typename U>
struct [[gnu::packed]] patch_entry {
	T *target;
	size_t size;
	U value;
};

template<typename ...T, typename ...U>
inline constexpr auto patch_list(const std::pair<T*, U> &...patches)
{
	return std::make_tuple(patch_entry {
		patches.first,
		sizeof(patches.second),
		patches.second
	}...);
}
#else
// Apply patches in constructor
#define PATCH_LIST(...) \
	static const auto patches##__COUNTER__ = patch_list { __VA_ARGS__ }

struct patch_list {
	template<typename ...T, typename ...U>
	patch_list(const std::pair<T*, U> &...patches)
	{
		([&] {
			*(U*)patches.first = patches.second;
			DCStoreRange(patches.first, sizeof(U));
			ICInvalidateRange(patches.first, sizeof(U));
		}(), ...);
	}
};
#endif
#pragma once

#include "util/preprocessor.h"
#include <cstddef>
#include <ogc/cache.h>
#include <tuple>

#ifdef DOL
// Store patches to be applied by patch_dol.py
#define PATCH_LIST(...) \
	[[gnu::section(".patches"), gnu::used]] \
	static constinit auto CONCAT(patches, __COUNTER__) = static_patch_list(__VA_ARGS__)

template<typename T, typename U>
struct [[gnu::packed]] patch_entry {
	T *target;
	size_t size;
	U value;
};

template<typename ...T, typename ...U>
inline constexpr auto static_patch_list(const std::pair<T*, U> &...patches)
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
	[[gnu::used]] \
	static const auto CONCAT(patches, __COUNTER__) = runtime_patch_list { __VA_ARGS__ }
#endif

struct runtime_patch_list {
	template<typename ...T, typename ...U>
	runtime_patch_list(const std::pair<T*, U> &...patches)
	{
		([&] {
			*(U*)patches.first = patches.second;
			DCStoreRange((void*)patches.first, sizeof(U));
			ICInvalidateRange((void*)patches.first, sizeof(U));
		}(), ...);
	}
};
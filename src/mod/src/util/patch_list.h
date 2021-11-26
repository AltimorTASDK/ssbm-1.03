#pragma once

#include <gctypes.h>
#include <ogc/cache.h>
#include <tuple>

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
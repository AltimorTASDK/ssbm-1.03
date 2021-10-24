#pragma once

#include <gctypes.h>
#include <tuple>

struct patch_list {
	template<typename ...T, typename ...U>
	patch_list(const std::pair<T*, U> &...patches)
	{
		((*(U*)patches.first = patches.second), ...);
	}
};
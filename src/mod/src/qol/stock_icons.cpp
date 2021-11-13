#include "melee/match.h"
#include "melee/scene.h"
#include "util/vector.h"
#include "util/melee/ports.h"
#include <gctypes.h>

extern "C" const vec3 &orig_HUD_GetStockIconPosition(u32 port);
extern "C" const vec3 &hook_HUD_GetStockIconPosition(u32 port)
{
	// Only apply to vs mode
	if (SceneMajor != Scene_VsMode)
		return orig_HUD_GetStockIconPosition(port);

	const auto sorted = get_sorted_port_index(port);
	if (sorted == -1)
		return orig_HUD_GetStockIconPosition(port);

	if (MatchInfo_IsTeams() && !is_1v1()) {
		// Display each team together
		const auto team = sorted % 2;
		const auto index = sorted / 2;
		return orig_HUD_GetStockIconPosition(team * 2 + index);
	}

	// Center to p2/3 positions in singles
	return orig_HUD_GetStockIconPosition(sorted + 1);
}
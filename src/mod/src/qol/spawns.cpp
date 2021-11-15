#include "melee/match.h"
#include "melee/player.h"
#include "melee/scene.h"
#include "melee/stage.h"
#include "menu/stage_select.h"
#include "util/patch_list.h"
#include "util/melee/ports.h"
#include <gctypes.h>

// First two are singles or red P1/blue P1, second two are red P2/blue P2
constexpr vec2 bf_spawns[]  = { { -38.8000f, 35.2000f }, { 38.8000f, 35.2000f },
                                { -38.8000f,  5.0000f }, { 38.8000f,  5.0000f } };

constexpr vec2 dl_spawns[]  = { { -46.6000f, 37.2215f }, { 47.3891f, 37.3215f },
                                { -46.6000f,  5.0000f }, { 47.3891f,  5.0000f } };

constexpr vec2 fd_spawns[]  = { { -60.0000f, 10.0000f }, { 60.0000f, 10.0000f },
                                { -20.0000f, 10.0000f }, { 20.0000f, 10.0000f } };

constexpr vec2 fod_spawns[] = { { -41.2500f, 21.0000f }, { 41.2500f, 27.0000f },
                                { -41.2500f,  5.0000f }, { 41.2500f,  5.0000f } };

constexpr vec2 ps_spawns[]  = { { -40.0000f, 32.0000f }, { 40.0000f, 32.0000f },
                                { -40.0000f,  5.0000f }, { 40.0000f,  5.0000f } };

constexpr vec2 ys_spawns[]  = { { -42.0000f, 26.6000f }, { 42.0000f, 28.0000f },
                                { -42.0000f,  5.0000f }, { 42.0000f,  5.0000f } };

static const vec2 *get_spawn_list()
{
	switch(Stage_GetID()) {
	case Stage_BF:  return bf_spawns;
	case Stage_DL:  return dl_spawns;
	case Stage_FD:  return fd_spawns;
	case Stage_FoD: return fod_spawns;
	case Stage_PS:  return ps_spawns;
	case Stage_YS:  return ys_spawns;
	default:        return nullptr;
	}
}

extern "C" void orig_Stage_GetSpawnPoint(u32 port, vec3 *result);
extern "C" void hook_Stage_GetSpawnPoint(u32 port, vec3 *result)
{
	orig_Stage_GetSpawnPoint(port, result);

	// No singleplayer
	if (IsSinglePlayerMode() && SceneMajor != Scene_Training)
		return;

	// No P5/P6
	if (port >= 4)
		return;

	const auto *spawns = get_spawn_list();

	// Must have a spawn list for this stage
	if (spawns == nullptr)
		return;

	const auto spawn_index = get_sorted_port_index(port);

	// Spawn index is -1 if not 1v1/2v2
	if (spawn_index == -1)
		return;

	result->x = spawns[spawn_index].x;
	result->y = spawns[spawn_index].y;

	// Adjust spawns for platform heights if using modded FoD
	if (Stage_GetID() == Stage_FoD && !use_og_stage_select) {
		if (spawn_index == 0)
			result->y += 4.f;
		else if (spawn_index == 1)
			result->y -= 4.f;
	}
}

extern "C" u32 orig_PlayerBlock_GetRespawnPoint(u32 port, vec3 *respawn, vec3 *offset);
extern "C" u32 hook_PlayerBlock_GetRespawnPoint(u32 port, vec3 *respawn, vec3 *offset)
{
	if (Stage_GetID() != Stage_BF && Stage_GetID() != Stage_FD)
		return orig_PlayerBlock_GetRespawnPoint(port, respawn, offset);

	// Use a single merged respawn point on BF and FD
	Stage.use_unique_respawns = false;
	orig_PlayerBlock_GetRespawnPoint(port, respawn, offset);
	respawn->x = 0;

	return 0;
}

extern "C" void orig_Match_SetSpawnFacingDirections();
extern "C" void hook_Match_SetSpawnFacingDirections()
{
	// Always face center in 1p mode
	if (get_player_count() != 1)
		orig_Match_SetSpawnFacingDirections();
}
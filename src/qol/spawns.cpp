#include "melee/match.h"
#include "melee/player.h"
#include "melee/scene.h"
#include "melee/stage.h"
#include <gctypes.h>

// First two are singles or red P1/blue P1, second two are red P2/blue P2
constexpr vec2 bf_spawns[]  = { { -38.8000f, 35.2000f }, { 38.8000f, 35.2000f },
                                { -38.8000f,  5.0000f }, { 38.8000f,  5.0000f } };

constexpr vec2 dl_spawns[]  = { { -46.6000f, 37.2215f }, { 47.3891f, 37.3215f },
                                { -46.6000f,  5.0000f }, { 47.3891f,  5.0000f } };

constexpr vec2 fd_spawns[]  = { { -60.0000f, 10.0000f }, { 60.0000f, 10.0000f },
                                { -20.0000f, 10.0000f }, { 20.0000f, 10.0000f } };

// Modified to account for neutral platform heights (vanilla Y 21 & 27)
constexpr vec2 fod_spawns[] = { { -41.2500f, 25.0000f }, { 41.2500f, 23.0000f },
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

// Returns -1 if not 1v1
static int get_singles_spawn(u32 slot)
{
	auto count = 0;
	int spawn;

	for (u32 i = 0; i < 4; i++) {
		if (PlayerBlock_GetSlotType(i) == SlotKind_None)
			continue;
			
		if (i == slot)
			spawn = count;

		if (++count > 2)
			return -1;
	}

	return spawn;
}

// Returns -1 if not 2v2
static int get_doubles_spawn(u32 slot)
{
	auto team1 = -1;
	auto team1_count = 1;
	auto team2 = -1;
	auto team2_count = 1;

	int spawn;
	int spawn_team;

	for (u32 i = 0; i < 4; i++) {
		if (PlayerBlock_GetSlotType(i) == SlotKind_None)
			continue;
			
		auto team = PlayerBlock_GetTeam(i);
		int index;

		if (team1 == -1) {
			team1 = team;
			index = 0;
		} else if (team == team1) {
			team1_count++;
			index = 2;
		} else if (team2 == -1) {
			team2 = team;
			index = 0;
		} else if (team == team2) {
			team2_count++;
			index = 2;
		} else {
			// Third team
			return -1;
		}
		
		if (i == slot) {
			spawn = index;
			spawn_team = team;
		}
	}

	if (team1_count != 2 || team2_count != 2)
		return -1; // Not 2v2
		
	// Check if on higher team index
	if (spawn_team * 2 > team1 + team2)
		spawn++;

	return spawn;
}

extern "C" void orig_Stage_GetSpawnPoint(u32 slot, vec3 *result);
extern "C" void hook_Stage_GetSpawnPoint(u32 slot, vec3 *result)
{
	orig_Stage_GetSpawnPoint(slot, result);
	
	// No singleplayer except for training mode
	if (IsSinglePlayerMode() && SceneMajor != Scene_Training)
		return;
	
	// No P5/P6
	if (slot >= 4)
		return;
		
	// Must have a spawn list for this stage
	const auto *spawns = get_spawn_list();
	if (spawns == nullptr)
		return;
		
	int spawn_index;
	if (MatchInfo_IsTeams())
		spawn_index = get_doubles_spawn(slot);
	else
		spawn_index = get_singles_spawn(slot);
		
	// Spawn index is -1 if not 1v1/2v2
	if (spawn_index != -1) {
		result->x = spawns[spawn_index].x;
		result->y = spawns[spawn_index].y;
	}
}
#include "melee/match.h"
#include "melee/player.h"
#include "melee/scene.h"
#include <gctypes.h>

// Returns -1 if not 1v1
static int get_singles_index(u32 port)
{
	auto count = 0;
	int result;

	for (u32 i = 0; i < 4; i++) {
		if (PlayerBlock_GetSlotType(i) == SlotType_None)
			continue;
			
		if (i == port)
			result = count;

		if (++count > 2)
			return -1;
	}
	
	if (count != 2)
		return -1;

	return result;
}

// Returns -1 if not 2v2
static int get_doubles_index(u32 port)
{
	auto team1 = -1;
	auto team1_count = 1;
	auto team2 = -1;
	auto team2_count = 1;

	int result;

	for (u32 i = 0; i < 4; i++) {
		if (PlayerBlock_GetSlotType(i) == SlotType_None)
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
		
		if (i == port)
			result = index;
	}

	if (team1_count != 2 || team2_count != 2)
		return -1; // Not 2v2
		
	// Check if on higher team index
	if (PlayerBlock_GetTeam(port) * 2 > team1 + team2)
		result++;

	return result;
}

bool is_1v1()
{
	auto count = 0;

	for (u32 i = 0; i < 4; i++) {
		if (PlayerBlock_GetSlotType(i) == SlotType_None)
			continue;
	}
	
	if (count != 2)
		return -1;

	return true;
}

// Returns -1 if not 1v1/2v2
int get_sorted_port_index(u32 port)
{
	if (MatchInfo_IsTeams() && SceneMajor != Scene_Training && !is_1v1())
		return get_doubles_index(port);
	else
		return get_singles_index(port);
}
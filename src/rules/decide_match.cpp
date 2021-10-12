#include "melee/match.h"
#include "melee/player.h"
#include "melee/rules.h"
#include "melee/scene.h"
#include "rules/values.h"

struct VictoryScreenInitData {
	char pad000[0x08];
	MatchController match_controller;
};

bool lgl_violated(int slot)
{
	const auto lgl = get_ledge_grab_limit();
	if (lgl == 0)
		return 0;
	
	const auto *stats = PlayerBlock_GetStats(slot);
	const auto ledge_grabs = PlayerBlockStats_GetActionStat(stats, ActionStat_LedgeGrabs);
	return ledge_grabs > lgl;
}

// Returns false if not 1v1
bool get_player_indices(int *p1, int *p2)
{
	auto count = 0;

	for (u32 i = 0; i < 4; i++) {
		if (PlayerBlock_GetSlotType(i) == SlotKind_None)
			continue;
			
		if (count == 0)
			*p1 = i;
		else if (count == 1)
			*p2 = i;
		else
			return false;

		count++;
	}
	
	return count == 2;
}

void set_winner(MatchController *controller, int winner, int loser)
{
	controller->players[winner].is_big_loser = false;
	controller->players[loser].is_big_loser = true;
}

bool enforce_rules(MatchController *controller)
{
	// Only check for timeouts
	if (controller->result != MatchResult_Timeout)
		return false;
	
	// Must be 1v1
	if (MatchInfo_IsTeams())
		return false;

	int p1, p2;
	if (!get_player_indices(&p1, &p2))
		return false;
		
	const auto p1_violated = lgl_violated(p1);
	const auto p2_violated = lgl_violated(p2);
		
	if (!p1_violated && p2_violated)
		set_winner(controller, p1, p2);
	else if (!p2_violated && p1_violated)
		set_winner(controller, p2, p1);
		
	return p1_violated || p2_violated;
}

extern "C" void orig_VictoryScreen_Init(VictoryScreenInitData *data);
extern "C" void hook_VictoryScreen_Init(VictoryScreenInitData *data)
{
	// Skip victory screen if no rule enforcement
	if (!enforce_rules(&data->match_controller))
		EndScene();
	
	orig_VictoryScreen_Init(data);
}
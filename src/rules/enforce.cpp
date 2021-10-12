#include "melee/match.h"
#include "melee/player.h"
#include "melee/rules.h"
#include "rules/values.h"

int get_excess_ledge_grabs(int slot)
{
	const auto lgl = get_ledge_grab_limit();
	if (lgl == 0)
		return 0;
	
	const auto *stats = PlayerBlock_GetStats(slot);
	const auto ledge_grabs = PlayerBlockStats_GetActionStat(stats, ActionStat_LedgeGrabs);
	return std::max(0, ledge_grabs - lgl);
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

void set_winner(int winner, int loser)
{
	LastMatchInfo->players[winner].is_big_loser = false;
	LastMatchInfo->players[loser].is_big_loser = true;
}

void enforce_rules()
{
	// Must be 1v1
	if (MatchInfo_IsTeams())
		return;

	int p1, p2;
	if (!get_player_indices(&p1, &p2))
		return;
		
	int p1_excess_lg = get_excess_ledge_grabs(p1);
	int p2_excess_lg = get_excess_ledge_grabs(p2);
		
	if (p1_excess_lg == 0 && p2_excess_lg != 0)
		set_winner(p1, p2);
	else if (p2_excess_lg == 0 && p1_excess_lg != 0)
		set_winner(p2, p1);
}

extern "C" void orig_VictoryScreen_Init(void *data);
extern "C" void hook_VictoryScreen_Init(void *data)
{
	enforce_rules();
	orig_VictoryScreen_Init(data);
}
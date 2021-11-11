#include "melee/match.h"
#include "melee/player.h"
#include "melee/rules.h"
#include "util/melee/match.h"

static bool won_last_match_teams(const MatchController &match, u32 port)
{
	// LRAS counts as team loss
	if (match.result == MatchResult_LRAStart)
		return match.players[port].team != match.players[GetMatchInfo()->pauser].team;
	
	for (auto i = 0; i < match.team_winner_count; i++) {
		if (match.team_winners[i] == match.players[port].team)
			return true;
	}

	return false;
}

static bool won_last_match_singles(const MatchController &match, u32 port)
{
	// LRAS counts as loss
	if (match.result == MatchResult_LRAStart)
		return port != GetMatchInfo()->pauser;

	for (auto i = 0; i < match.winner_count; i++) {
		if (match.winners[i] == port)
			return true;
	}

	return false;
}

bool won_last_match(const MatchController &match, u32 port)
{
	// Must have participated in last match
	if (match.players[port].slot_type == SlotType_None)
		return false;
		
	return match.is_teams ? won_last_match_teams(match, port)
	                      : won_last_match_singles(match, port);
}

void update_crew_stocks(const MatchController &match)
{
	last_stock_count = GetGameRules()->stock_count;

	for (auto i = 0; i < 4; i++) {
		if (won_last_match(match, i))
			crew_stocks[i] = match.players[i].stocks;
		else
			crew_stocks[i] = -1;
	}

	for (auto i = 0; i < 4; i++) {
		if (PlayerBlock_GetSlotType(i) != SlotType_None && !won_last_match(i))
			return;
	}
	
	// Reset crew stocks if a draw occurs
	reset_crew_stocks();
}
#include "melee/match.h"
#include "melee/player.h"
#include "melee/rules.h"

static bool won_last_match_teams(u32 port)
{
	const auto &match = LastMatchData.match;

	// LRAS counts as team loss
	if (match.result == MatchResult_LRAStart)
		return match.players[port].team != match.players[GetMatchInfo()->pauser].team;
	
	for (auto i = 0; i < match.team_winner_count; i++) {
		if (match.team_winners[i] == match.players[port].team)
			return true;
	}

	return false;
}

static bool won_last_match_singles(u32 port)
{
	const auto &match = LastMatchData.match;

	// LRAS counts as loss
	if (match.result == MatchResult_LRAStart)
		return port != GetMatchInfo()->pauser;

	for (auto i = 0; i < match.winner_count; i++) {
		if (match.winners[i] == port)
			return true;
	}

	return false;
}

bool won_last_match(u32 port)
{
	const auto &match = LastMatchData.match;
	
	// Must have participated in last match
	if (match.players[port].slot_type == SlotType_None)
		return false;
		
	return match.is_teams ? won_last_match_teams(port)
	                      : won_last_match_singles(port);
}
#include "melee/match.h"
#include "melee/player.h"
#include "melee/scene.h"
#include "melee/text.h"

constexpr auto winner_color = color_rgba(255, 215, 0, 255);

static bool won_last_match_teams(const MatchInfo *match_info, int port)
{
	const auto &match = LastMatchData.match;

	// LRAS counts as team loss
	if (match.result == MatchResult_LRAStart)
		return match.players[port].team != match.players[match_info->pauser].team;
	
	for (auto i = 0; i < match.team_winner_count; i++) {
		if (match.team_winners[i] == match.players[port].team)
			return true;
	}

	return false;
}

static bool won_last_match(const MatchInfo *match_info, int port)
{
	const auto &match = LastMatchData.match;

	// LRAS counts as loss
	if (match.result == MatchResult_LRAStart)
		return port != match_info->pauser;

	for (auto i = 0; i < match.winner_count; i++) {
		if (match.winners[i] == port)
			return true;
	}

	return false;
}

extern "C" void set_css_name_color(int port, Text *text)
{
	// Don't display in singleplayer modes
	if (IsSinglePlayerMode())
		return;
	
	const auto *match_info = GetMatchInfo();
	const auto &match = LastMatchData.match;
	
	// Must have participated in last match
	if (match.players[port].slot_type == SlotType_None)
		return;
		
	const auto won = match.is_teams ? won_last_match_teams(match_info, port)
	                                : won_last_match(match_info, port);
					
	if (won)
		Text_SetSubtextColor(text, 0, winner_color);
}
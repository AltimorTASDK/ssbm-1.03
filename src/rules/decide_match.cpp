#include "melee/match.h"
#include "melee/player.h"
#include "melee/rules.h"
#include "melee/scene.h"
#include "rules/values.h"
#include <algorithm>

struct MatchExitData {
	char pad000[0x0C];
	MatchController match;
};

struct score {
	int id;
	bool violation;
	int stocks;
	int percent;
};

static bool lgl_violated(int slot)
{
	const auto lgl = get_ledge_grab_limit();
	if (lgl == 0)
		return 0;
	
	const auto *stats = PlayerBlock_GetStats(slot);
	const auto ledge_grabs = PlayerBlockStats_GetActionStat(stats, ActionStat_LedgeGrabs);
	return ledge_grabs > lgl;
}

static bool is_score_better(const score &a, const score &b)
{
	if (!a.violation && b.violation)
		return true;

	if (a.stocks > b.stocks)
		return true;
	
	if (a.percent < b.percent)
		return true;
		
	return false;
}

static bool is_score_equal(const score &a, const score &b)
{
	if (a.violation != b.violation)
		return false;

	if (a.stocks != b.stocks)
		return false;
	
	if (a.percent != b.percent)
		return false;
		
	return true;
}

static score add_score(const score &a, const score &b)
{
	return {
		.id = a.id,
		.violation = a.violation || b.violation,
		.stocks = a.stocks + b.stocks,
		.percent = a.percent + b.percent
	};
}

// Returns count
static int get_player_scores(MatchController *match, score *scores, bool *violation)
{
	auto count = 0;
	*violation = false;

	for (auto i = 0; i < 6; i++) {
		if (match->players[i].slot_type == SlotType_None)
			continue;

		scores[count] = {
			.id = i,
			.violation = lgl_violated(i),
			.stocks = match->players[i].score,
			.percent = match->players[i].percent
		};
		
		if (scores[count].violation)
			*violation = true;
		
		count++;
	}
	
	return count;
}

static void set_player_result(MatchController *match, int slot, bool win)
{
	match->players[slot].is_big_loser = !win;
	if (win) {
		match->winners[match->winner_count] = (u8)slot;
		match->winner_count++;
	}
}

static bool decide_winners(MatchController *match)
{
	// Only apply for timeouts
	if (match->result != MatchResult_Timeout)
		return false;
		
	// Don't check singles win conditions in teams
	if(match->is_teams)
		return false;

	score scores[6];
	bool violation;
	const auto count = get_player_scores(match, scores, &violation);

	// We only care about the highest score
	std::partial_sort(scores, scores + 1, scores + count, is_score_better);
	
	// Everyone tied for first wins
	set_player_result(match, scores[0].id, true);
	
	for (auto i = 1; i < count; i++) {
		const auto win = is_score_equal(scores[i], scores[0]);
		set_player_result(match, scores[i].id, win);
	}
		
	// Custom result type
	if (violation)
		match->result = MatchResult_RuleViolation;
	
	return true;
}

extern "C" void orig_MatchController_UpdateWinners(MatchController *match);
extern "C" void hook_MatchController_UpdateWinners(MatchController *match)
{
	if (!decide_winners(match))
		orig_MatchController_UpdateWinners(match);
}

extern "C" void orig_Scene_Match_Exit(SceneMinorData *data, u8 victory_screen, u8 sudden_death);
extern "C" void hook_Scene_Match_Exit(SceneMinorData *data, u8 victory_screen, u8 sudden_death)
{
	const auto *exit_data = (MatchExitData*)data->exit_data;

	// Skip victory screen if no rule enforcement and never enter sudden death
	if (exit_data->match.result != MatchResult_RuleViolation)
		orig_Scene_Match_Exit(data, VsScene_CSS, VsScene_CSS);
	else
		orig_Scene_Match_Exit(data, victory_screen, victory_screen);
}
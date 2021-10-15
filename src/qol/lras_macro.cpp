#include "hsd/pad.h"
#include "melee/match.h"
#include "melee/scene.h"

static bool released_start = false;

bool check_lras_macro()
{
	if  (!Scene_CheckPauseFlag(PauseBit_Pause)) {
		released_start = false;
		return false;
	} else if (released_start) {
		return false;
	}

	const auto *match_info = GetMatchInfo();
	const auto port = match_info->pauser;
	const auto &pad = HSD_PadMasterStatus[port];

	if (!(pad.buttons & Button_Start) || (pad.buttons & (Button_X | Button_Y))) {
		// Player must hold pause throughout LRAS delay
		// Additionally, don't apply macro if X/Y is held to allow for X+Y+Start
		released_start = true;
		return false;
	}
	
	return match_info->pause_timer == 0;
}

extern "C" void orig_Match_HandlePause();
extern "C" void hook_Match_HandlePause()
{
	if (check_lras_macro())
		Match_NoContestOrRetry(0, MatchResult_LRAStart);
	else
		orig_Match_HandlePause();
}
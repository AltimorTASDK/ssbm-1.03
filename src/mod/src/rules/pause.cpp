#include "hsd/pad.h"
#include "melee/match.h"
#include "melee/player.h"
#include "melee/rules.h"

static bool multiple_humans()
{
	auto count = 0;

	for (auto i = 0; i < 6; i++) {
		if (PlayerBlock_GetSlotType(i) != SlotType_Human)
			continue;

		const auto port = PlayerBlock_GetPort(i);

		if (HSD_PadMasterStatus[port].err == 0 && ++count > 1)
			return true;
	}

	return false;
}

extern "C" void orig_Match_CheckToPause(MatchInfo *match, u32 pause_bit);
extern "C" void hook_Match_CheckToPause(MatchInfo *match, u32 pause_bit)
{
	if (match->rules.pause_disabled && !multiple_humans()) {
		// Allow pause if only one human is plugged in
		match->rules.pause_disabled = false;
		orig_Match_CheckToPause(match, pause_bit);
		match->rules.pause_disabled = true;
		return;
	}

	orig_Match_CheckToPause(match, pause_bit);
}

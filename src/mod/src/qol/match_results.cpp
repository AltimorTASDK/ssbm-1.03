#include "hsd/pad.h"
#include "melee/match.h"
#include "melee/player.h"
#include "melee/rules.h"
#include "melee/scene.h"
#include "util/melee/match.h"
#include <gctypes.h>

static u8 get_lras_override_scene()
{
	const auto *match_info = GetMatchInfo();
	const auto port = match_info->pauser;
	const auto &pad = HSD_PadMasterStatus[port];

	// Hold B for salty runback
	if (pad.buttons & Button_B)
		return VsScene_Game;
		
	return VsScene_CSS;
}

static u8 get_next_scene(MatchExitData *exit_data)
{
	// Add extra LRAS button combos
	if (exit_data->match.result == MatchResult_LRAStart)
		return get_lras_override_scene();

	// Show victory screen if timeout
	if (exit_data->match.result == MatchResult_Timeout)
		return VsScene_Victory;

	return VsScene_CSS;
}

extern "C" void orig_Scene_Match_Exit(SceneMinorData *data, u8 victory_screen, u8 sudden_death);
extern "C" void hook_Scene_Match_Exit(SceneMinorData *data, u8 victory_screen, u8 sudden_death)
{
	// Override next scene and remove sudden death
	const auto next_scene = get_next_scene((MatchExitData*)data->data.exit_data);
		
	// Store crew stock count for next game
	if (GetGameRules()->mode == Mode_Crew)
		update_crew_stocks();

	orig_Scene_Match_Exit(data, next_scene, next_scene);
}
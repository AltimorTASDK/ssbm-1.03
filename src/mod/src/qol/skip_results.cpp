#include "melee/match.h"
#include "melee/scene.h"
#include <gctypes.h>

extern "C" void orig_Scene_Match_Exit(SceneMinorData *data, u8 victory_screen, u8 sudden_death);
extern "C" void hook_Scene_Match_Exit(SceneMinorData *data, u8 victory_screen, u8 sudden_death)
{
	const auto *exit_data = (MatchExitData*)data->exit_data;

	// Skip victory screen if no timeout and never enter sudden death
	if (exit_data->match.result != MatchResult_Timeout)
		orig_Scene_Match_Exit(data, VsScene_CSS, VsScene_CSS);
	else
		orig_Scene_Match_Exit(data, victory_screen, victory_screen);
}
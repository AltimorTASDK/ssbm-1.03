#include "melee/stage.h"
#include "menu/stage_select.h"

extern "C" void orig_Stage_Fountain_Init();
extern "C" void hook_Stage_Fountain_Init()
{
	if (!use_og_stage_select) {
		// Use the average of the default platform heights (20 and 28)
		auto *params = Stage_GetParameters();
		params->fod.left_plat_height = 24.f;
		params->fod.right_plat_height = 24.f;
	}

	orig_Stage_Fountain_Init();
}
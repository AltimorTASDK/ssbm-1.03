#include "melee/stage.h"

extern "C" void orig_Stage_Fountain_Init();
extern "C" void hook_Stage_Fountain_Init()
{
	// Use the average of the default platform heights (20 and 28)
	auto *params = Stage_GetParameters();
	params->fod.left_plat_height = 24.f;
	params->fod.right_plat_height = 24.f;

	orig_Stage_Fountain_Init();
}
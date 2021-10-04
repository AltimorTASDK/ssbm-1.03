#include "hsd/gobj.h"
#include "melee/stage.h"

extern "C" void orig_Stage_Dreamland_WhispyUpdate(HSD_GObj *gobj);
extern "C" void hook_Stage_Dreamland_WhispyUpdate(HSD_GObj *gobj)
{
	// Prevent Whispy from blowing
	auto *whispy = gobj->get<StageObject>();
	whispy->push_timer = 1200;

	orig_Stage_Dreamland_WhispyUpdate(gobj);
}
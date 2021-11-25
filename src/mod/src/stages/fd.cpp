#include "hsd/gobj.h"
#include "rules/values.h"

extern "C" void orig_Stage_FinalDestination_BackgroundUpdate(HSD_GObj *gobj);
extern "C" void hook_Stage_FinalDestination_BackgroundUpdate(HSD_GObj *gobj)
{
	// Do nothing if not original
	if (!is_stage_frozen(Stage_FD))
		orig_Stage_FinalDestination_BackgroundUpdate(gobj);
}
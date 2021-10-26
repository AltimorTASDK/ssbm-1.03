#include "hsd/gobj.h"
#include "menu/stage_select.h"

extern "C" void orig_Stage_YoshisStory_ShyGuyUpdate(HSD_GObj *gobj);
extern "C" void hook_Stage_YoshisStory_ShyGuyUpdate(HSD_GObj *gobj)
{
	// Do nothing if not original
	if (use_og_stage_select)
		orig_Stage_YoshisStory_ShyGuyUpdate(gobj);
}
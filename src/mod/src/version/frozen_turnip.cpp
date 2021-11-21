#ifdef NTSC100

#include "hsd/gobj.h"
#include "melee/item.h"
#include <gctypes.h>

extern "C" void orig_Item_SetVisible(HSD_GObj *gobj);
extern "C" void hook_Item_SetVisible(HSD_GObj *gobj)
{
	orig_Item_SetVisible(gobj);

	// Re-enable physics
	gobj->get<Item>()->no_update = 0;
}

#endif
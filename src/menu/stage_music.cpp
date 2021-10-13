#include "hsd/jobj.h"
#include <gctypes.h>

struct ItemMenuData {
	
};

extern "C" HSD_JObj *orig_Menu_CreateItemToggle(u32 index, ItemMenuData *data);
extern "C" HSD_JObj *hook_Menu_CreateItemToggle(u32 index, ItemMenuData *data)
{
	auto *jobj = orig_Menu_CreateItemToggle(index, data);
	
	// Hide item icon
	//auto *icon = HSD_JObjGetFromTreeByIndex(jobj, 7);
	//HSD_JObjSetFlagsAll(jobj, HIDDEN);
	
	return jobj;
}
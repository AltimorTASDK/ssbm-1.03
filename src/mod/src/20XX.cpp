#include "hsd/gobj.h"
#include "util/patch_list.h"

extern "C" char SaveFileName[25];

extern "C" void SSS_CursorThink(HSD_GObj *gobj);

static const auto result = [] {
	// XXperSmashBros
	if (SaveFileName[0] != 'X' || SaveFileName[1] != 'X')
		return false;

	patch_list {
		// Get rid of 20XX's stage striking hook
		// lfs f1, -0x3618(r2)
		std::pair { (char*)SSS_CursorThink+0xAC, 0xC022C9E8u }
	};

	return true;
}();

extern "C" bool is_20XX()
{
	return result;
}
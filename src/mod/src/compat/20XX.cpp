#include "hsd/gobj.h"
#include "hsd/pad.h"
#include "util/patch_list.h"

extern "C" char SaveFileName[25];
extern "C" char MnSlMapPath[];

// XXperSmashBros
static const auto result = SaveFileName[0] == 'X' && SaveFileName[1] == 'X';

extern "C" bool is_20XX()
{
	return result;
}

extern "C" bool is_20XX_stage_select()
{
	// 20XX loads MnSlMap.1sd, MnSlMap.2sd etc
	return is_20XX() && MnSlMapPath[8] != '1' && MnSlMapPath[8] != 'u';
}

extern "C" void orig_SSS_CursorThink(HSD_GObj *gobj);
extern "C" void hook_SSS_CursorThink(HSD_GObj *gobj)
{
	if (!is_20XX())
		return orig_SSS_CursorThink(gobj);

	// Prevent 20XX from receiving X/Y/Z presses for stage striking
	auto *queue = HSD_PadLibData.queue;
	u16 old_buttons[4];

	for (auto port = 0; port < 4; port++) {
		old_buttons[port] = queue[0].stat[port].buttons;
		queue[0].stat[port].buttons &= (u16)~(Button_X | Button_Y | Button_Z);
	}

	orig_SSS_CursorThink(gobj);

	for (auto port = 0; port < 4; port++)
		queue[0].stat[port].buttons = old_buttons[port];
}
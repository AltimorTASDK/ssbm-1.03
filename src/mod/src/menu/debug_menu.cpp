#include "compat/20XX.h"
#include "hsd/pad.h"
#include "melee/debug.h"
#include "melee/menu.h"
#include "melee/preferences.h"
#include "melee/scene.h"
#include <cstring>
#include <gctypes.h>

enum DebugButton {
	DebugButton_Up    = 0x10000000,
	DebugButton_Down  = 0x20000000,
	DebugButton_Left  = 0x40000000,
	DebugButton_Right = 0x80000000
};

static s32 repeat_timer[4];

extern "C" u32 get_directional_input(int port)
{
	constexpr auto threshold = 60;

	const auto &pad = HSD_PadCopyStatus[port];

	u32 buttons = 0;

	if (pad.raw_stick.x < -threshold)
		buttons |= DebugButton_Left;
	else if (pad.raw_stick.x > threshold)
		buttons |= DebugButton_Right;

	if (pad.raw_stick.y < -threshold)
		buttons |= DebugButton_Down;
	else if (pad.raw_stick.y > threshold)
		buttons |= DebugButton_Up;

	if (buttons == 0) {
		repeat_timer[port] = 0;
	} else if (repeat_timer[port] != 0) {
		repeat_timer[port]--;
		return 0;
	} else {
		repeat_timer[port] = (pad.buttons & Button_R) ? 1 : 8;
	}

	return buttons;
}

extern "C" u32 get_debug_menu_buttons()
{
	// Check all ports
	u32 buttons = 0;
	for (auto i = 0; i < 4; i++) {
		buttons |= HSD_PadCopyStatus[i].instant_buttons;
		buttons |= get_directional_input(i);
	}

	return buttons;
}

extern "C" u32 orig_DebugMenu_Exit(u32 arg);
extern "C" u32 hook_DebugMenu_Exit(u32 arg)
{
	if (is_20XX())
		return orig_DebugMenu_Exit(arg);

	// Exit back to VS menu
	if (arg == 0) {
		Menu_PlaySFX(MenuSFX_Back);
		Scene_SetMajorPending(Scene_Menu);
		Scene_Exit();
	}

	return 0;
}

extern "C" void orig_DebugMenu_Init(SceneMinorData *data);
extern "C" void hook_DebugMenu_Init(SceneMinorData *data)
{
	if (!is_20XX()) {
		// Set debug menu language to game language
		*DebugMenuEntries[DebugEntry_Language].value = GetSavedPreferences()->language;
	}

	orig_DebugMenu_Init(data);
}
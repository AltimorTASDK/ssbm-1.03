#include "melee/menu.h"
#include "melee/scene.h"
#include <gctypes.h>

enum DebugEntry {
	DebugEntry_Language = 3
};

struct MainMenuEnterData {
	u8 menu_type;
	u8 selected;
	u8 pad002;
};

struct DebugMenuEntry {
	u32 type;
	void(*on_change)();
	char *label;
	char **options;
	u32 *value;
	char pad014[0x20 - 0x14];
};

extern "C" DebugMenuEntry DebugMenuEntries[10];

extern "C" bool IsLanguageUS();

extern "C" void orig_MainMenu_Enter(SceneMinorData *minor);
extern "C" void hook_MainMenu_Enter(SceneMinorData *minor)
{
	orig_MainMenu_Enter(minor);
	
	// Go back to debug menu portal when returning from debug menu
	if (SceneMajorPrevious == Scene_DebugMenu) {
		auto *data = (MainMenuEnterData*)minor->enter_data;
		data->menu_type = MenuType_VsMode;
		data->selected = 1;
	}
}

extern "C" u32 hook_DebugMenu_Exit(u32 arg)
{
	// Exit back to VS menu
	if (arg == 0) {
		Menu_PlaySFX(MenuSFX_Enter);
		Scene_SetMajorPending(Scene_Menu);
		Scene_Exit();
	}

	return 0;
}

struct set_debug_language {
	set_debug_language()
	{
		// Set default debug menu language to game language
		*DebugMenuEntries[DebugEntry_Language].value = IsLanguageUS();
	}
} set_debug_language;
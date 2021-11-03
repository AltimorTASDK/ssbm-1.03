#include "melee/menu.h"
#include "melee/scene.h"

struct MainMenuEnterData {
	u8 menu_type;
	u8 selected;
	u8 pad002;
};

extern "C" void orig_MainMenu_Enter(SceneMinorData *minor);
extern "C" void hook_MainMenu_Enter(SceneMinorData *minor)
{
	orig_MainMenu_Enter(minor);

	auto *data = (MainMenuEnterData*)minor->data.enter_data;
	
	// Go back to debug menu portal when returning from debug menu
	if (SceneMajorPrevious == Scene_DebugMenu) {
		data->menu_type = MenuType_VsMode;
		data->selected = 1;
	}

	// Hover over controls portal when returning from controls menu
	if (SceneMajorPrevious == Scene_Controls)
		data->selected = 2;
}
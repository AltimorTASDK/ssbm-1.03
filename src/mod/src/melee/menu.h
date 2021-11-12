#pragma once

#include <gctypes.h>

constexpr auto PORT_ALL = 4;

enum MenuButton {
	MenuButton_Up      = 0x001,
	MenuButton_Down    = 0x002,
	MenuButton_Left    = 0x004,
	MenuButton_Right   = 0x008,
	MenuButton_Confirm = 0x010,
	MenuButton_B       = 0x020,
	MenuButton_L       = 0x040,
	MenuButton_R       = 0x080,
	MenuButton_Start   = 0x100,
	MenuButton_A       = 0x200,
	MenuButton_X       = 0x400,
	MenuButton_Y       = 0x800
};

enum MenuSFX {
	MenuSFX_Back      = 0,
	MenuSFX_Activate  = 1,
	MenuSFX_Scroll    = 2,
	MenuSFX_Denied    = 3
};

enum MenuState {
	MenuState_Idle      = 0,
	MenuState_EnterTo   = 1,
	MenuState_ExitFrom  = 2,
	MenuState_ExitTo    = 3,
	MenuState_EnterFrom = 4
};

enum MenuID {
	MenuID_Tournament = 36,
	MenuID_Controls   = 36,
	MenuID_None       = 45
};

enum MenuType {
	MenuType_VsMode = 2,
	MenuType_Rules = 13,
	MenuType_ExtraRules = 15,
	MenuType_ItemSwitch = 16,
	MenuType_StageMusic = 16,
	MenuType_RandomStage = 17,
	MenuType_MenuMusic = 17,
	MenuType_NameEntry = 18,
	MenuType_Max = 34
};

struct MenuCallbacks {
	u8 id;
	void(*think)();
	void(*enter)(void *enter_data);
	void(*exit)(void *exit_data);
	void *pad010;
};

extern "C" {
	
extern u16 MenuInputCooldown;

extern u8 MenuType;
extern u8 MenuTypePrevious;

extern u16 MenuSelectedIndex;
extern u8 MenuSelectedValue;

extern u32 MenuButtons;

MenuCallbacks *Menu_GetCallbacks(u8 id);

void Menu_ExitToMinorScene(u8 scene);
u32 Menu_GetButtons(u32 port);
void Menu_PlaySFX(s32 sfx);

void Menu_ExitToRulesMenu();
void Menu_CreateRandomStageMenu();
void Menu_MainMenuTransition(u32 menu_type, u16 index, u8 state);

}

// Call Menu_GetButtons without affecting the input cooldown
inline u32 Menu_GetButtonsHelper(u32 port)
{
	const auto cooldown = MenuInputCooldown;
	const auto buttons = Menu_GetButtons(port);
	MenuInputCooldown = cooldown;
	return buttons;
}
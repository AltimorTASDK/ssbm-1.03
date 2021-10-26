#pragma once

#include <gctypes.h>

enum MenuButton {
	MenuButton_Up    = 0x001,
	MenuButton_Down  = 0x002,
	MenuButton_Left  = 0x004,
	MenuButton_Right = 0x008,
	MenuButton_B     = 0x020,
	MenuButton_L     = 0x040,
	MenuButton_R     = 0x080,
	MenuButton_Start = 0x100,
	MenuButton_A     = 0x200,
	MenuButton_X     = 0x400,
	MenuButton_Y     = 0x800
};

enum MenuSFX {
	MenuSFX_Enter     = 0,
	MenuSFX_Exit      = 1,
	MenuSFX_Activated = 2,
	MenuSFX_Denied    = 3
};

enum MenuState {
	MenuState_Idle      = 0,
	MenuState_EnterTo   = 1,
	MenuState_ExitFrom  = 2,
	MenuState_ExitTo    = 3,
	MenuState_EnterFrom = 4
};

enum MenuType {
	MenuType_Rules = 13,
	MenuType_ExtraRules = 15,
	MenuType_ItemSwitch = 16,
	MenuType_StageMusic = 16,
	MenuType_RandomStage = 17,
	MenuType_MenuMusic = 17,
	MenuType_Max = 34
};

extern "C" {

extern u8 MenuType;
extern u8 MenuTypePrevious;

extern u16 MenuSelectedIndex;
extern u8 MenuSelectedValue;

extern u32 MenuButtons;

u32 Menu_GetButtons(u32 index);
void Menu_PlaySFX(s32 sfx);

void Menu_ExitToRulesMenu();
void Menu_CreateRandomStageMenu();

}
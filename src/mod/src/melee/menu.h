#pragma once

#include <gctypes.h>

enum MenuButton {
	MenuButton_Up    = 0x001,
	MenuButton_Down  = 0x002,
	MenuButton_Left  = 0x004,
	MenuButton_Right = 0x008,
	MenuButton_A     = 0x010,
	MenuButton_B     = 0x020,
	MenuButton_L     = 0x040,
	MenuButton_R     = 0x080,
	MenuButton_Start = 0x100,
	MenuButton_X     = 0x400,
	MenuButton_Y     = 0x800
};

enum MenuSFX {
	MenuSFX_Enter     = 0,
	MenuSFX_Exit      = 1,
	MenuSFX_Activated = 2,
	MenuSFX_Denied    = 3
};

extern "C" {

extern u16 MenuSelectedIndex;
extern u8 MenuSelectedValue;

u32 Menu_GetButtons(u32 index);
void Menu_PlaySFX(s32 sfx);

}
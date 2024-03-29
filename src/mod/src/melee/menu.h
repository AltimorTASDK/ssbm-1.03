#pragma once

#include "hsd/gobj.h"
#include "hsd/jobj.h"
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
	MenuSFX_Error     = 3
};

enum MenuState {
	MenuState_Idle      = 0,
	MenuState_EnterTo   = 1,
	MenuState_ExitFrom  = 2,
	MenuState_ExitTo    = 3,
	MenuState_EnterFrom = 4
};

enum MenuID {
#ifdef PAL
	MenuID_Tournament = 38,
	MenuID_Controls   = 38,
	MenuID_None       = 47
#else
	MenuID_Tournament = 36,
	MenuID_Controls   = 36,
	MenuID_None       = 45
#endif
};

enum MenuType {
	MenuType_Main         = 0,
	MenuType_VsMode       = 2,
	MenuType_Options      = 4,
	MenuType_SpecialMelee = 12,
	MenuType_Rules        = 13,
	MenuType_Manual       = 14,
	MenuType_ExtraRules   = 15,
	MenuType_ItemSwitch   = 16,
	MenuType_StageMusic   = 16,
	MenuType_RandomStage  = 17,
	MenuType_MenuMusic    = 17,
	MenuType_NameEntry    = 18,
	MenuType_Language     = 23,
	MenuType_DebugMode    = 23,
	MenuType_Max          = 34
};

enum MainMenuPortalID {
	MainMenu_1PMode   = 0,
	MainMenu_VSMode   = 1,
	MainMenu_Trophies = 2,
	MainMenu_Options  = 3,
	MainMenu_Data     = 4
};

enum VsMenuPortalID {
	VsMenu_Melee        = 0,
	VsMenu_Tournament   = 1,
	VsMenu_Controls     = 1,
	VsMenu_SpecialMelee = 2,
	VsMenu_Options      = 2,
	VsMenu_CustomRules  = 3,
	VsMenu_DebugMenu    = 3,
	VsMenu_NameEntry    = 4,
	VsMenu_Manual       = 4,
};

enum OptionsMenuPortalID {
	OptionsMenu_Rumble        = 0,
	OptionsMenu_Sound         = 1,
	OptionsMenu_ScreenDisplay = 2,
	OptionsMenu_Language      = 4,
	OptionsMenu_EraseData     = 5
};


struct MenuCallbacks {
	u8 id;
	void(*think)();
	void(*enter)(void *enter_data);
	void(*exit)(void *exit_data);
	void *pad010;
};

struct MenuTypeData {
	HSD_AnimLoop *preview_anims;
	f32 anim_frame;
	u16 *descriptions;
	u8 option_count;
	void(*think)(HSD_GObj *gobj);
};

extern "C" {

extern u16 MenuInputCooldown;

extern u8 MenuType;
extern u8 MenuTypePrevious;

extern u16 MenuSelectedIndex;
extern u8 MenuSelectedValue;

extern u32 MenuButtons;

extern u8 IsEnteringMenu;

extern MenuTypeData MenuTypeDataTable[MenuType_Max];

MenuCallbacks *Menu_GetCallbacks(u8 id);

void Menu_ExitToScene(u8 scene);
u32 Menu_GetButtons(u32 port);
void Menu_PlaySFX(s32 sfx);

void Menu_ExitToRulesMenu();
void Menu_CreateRandomStageMenu();
void Menu_CreateLanguageMenu(u8 state);
void Menu_EnterCustomRulesMenu();

// Frees current gobj
void Menu_MainMenuTransition(u32 menu_type, u16 index, u8 state);

void Menu_SetGObjPrio(HSD_GObj *gobj);

} // extern "C"

// Call Menu_GetButtons without affecting the input cooldown
inline u32 Menu_GetButtonsHelper(u32 port)
{
	const auto cooldown = MenuInputCooldown;
	const auto buttons = Menu_GetButtons(port);
	MenuInputCooldown = cooldown;
	return buttons;
}
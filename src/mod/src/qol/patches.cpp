#include "hsd/gobj.h"
#include "melee/player.h"
#include "melee/match.h"
#include "util/patch_list.h"
#include <gctypes.h>
#include <utility>

extern "C" void Camera_CStickControl();

extern "C" void DevelopMode_SetCameraType();

extern "C" void CSS_PlayerThink(HSD_GObj *gobj);
extern "C" void CSS_Input();
extern "C" void CSS_Setup();
extern "C" void CSS_UpdatePortrait(u8 port);
extern "C" void CSS_GObjCallback(u8 port);

extern "C" void SSS_Think();

extern "C" void MainMenu_Think();

extern "C" void TitleScreen_Think();

extern "C" void CreateScorePopup(u8 port);

extern "C" void NameTag_Think(HSD_GObj *gobj);

extern "C" void VsMenu_Think();

extern "C" void Damage_DamageStale();

extern "C" void MessageBox_CheckData();

extern "C" double CSSAnimStartFrame;

constexpr u32 NOP = 0x60000000;
constexpr u32 BLR = 0x4E800020;

PATCH_LIST(
	// Enable cstick in develop mode
	std::pair { PlayerThink_Input+0x188,       NOP },
	std::pair { PlayerThink_Input+0xF0,        NOP },
	// Enable cstick in singleplayer
	std::pair { PlayerThink_Input+0x194,       NOP },
	std::pair { PlayerThink_Input+0xFC,        NOP },
	// Disable develop mode camera control
	std::pair { DevelopMode_SetCameraType+0x0, BLR },
	// Disable cstick camera control
	std::pair { Camera_CStickControl+0x0,      BLR },
	// Close CSS port on unplug
	// li r6, SlotType_None
	std::pair { CSS_PlayerThink+0x35C,         0x38C00003u },
	// Allow setting any port to HMN at CSS
	// li r3, SlotType_Human
#ifdef PAL
	std::pair { CSS_PlayerThink+0x1848,        0x38600000u },
#else
	std::pair { CSS_PlayerThink+0x183C,        0x38600000u },
#endif
	// Apply random CPU character when opening fake HMN port
	// cmplwi r3, SlotType_Human
#ifdef PAL
	std::pair { CSS_PlayerThink+0x1870,        0x28030000u },
#else
	std::pair { CSS_PlayerThink+0x1864,        0x28030000u },
#endif
	// Disable handicap display in CSS
	// li r0, 0
	std::pair { CSS_Setup+0x256C,              0x38000000u },
	std::pair { CSS_UpdatePortrait+0x650,      0x38000000u },
	std::pair { CSS_UpdatePortrait+0x660,      0x38000000u },
	std::pair { CSS_UpdatePortrait+0x660,      0x38000000u },
	std::pair { CSS_UpdatePortrait+0x738,      0x38000000u },
	std::pair { CSS_UpdatePortrait+0x748,      0x38000000u },
	std::pair { CSS_UpdatePortrait+0x86C,      0x38000000u },
	std::pair { CSS_UpdatePortrait+0xAAC,      0x38000000u },
	std::pair { CSS_UpdatePortrait+0xACC,      0x38000000u },
	std::pair { CSS_UpdatePortrait+0xB0C,      0x38000000u },
	std::pair { CSS_UpdatePortrait+0xBF0,      0x38000000u },
	std::pair { CSS_GObjCallback+0x2B0,        0x38000000u },
	std::pair { CSS_GObjCallback+0x2CC,        0x38000000u },
	std::pair { CSS_GObjCallback+0x364,        0x38000000u },
	std::pair { CSS_GObjCallback+0x3F8,        0x38000000u },
	std::pair { CSS_PlayerThink+0xBA0,         0x38000000u },
#ifdef PAL
	std::pair { CSS_PlayerThink+0x198C,        0x38000000u },
	std::pair { CSS_PlayerThink+0x1A58,        0x38000000u },
#else
	std::pair { CSS_PlayerThink+0x1980,        0x38000000u },
	std::pair { CSS_PlayerThink+0x1A4C,        0x38000000u },
#endif
	// Disable score popups
	// li r3, NULL
	std::pair { CreateScorePopup+0x0,          0x38600000u },
	std::pair { CreateScorePopup+0x4,          BLR },
	// Remove in-game port tags (P1/P2 etc)
#ifdef PAL
	std::pair { NameTag_Think+0x7C,            NOP },
	std::pair { NameTag_Think+0x9C,            NOP },
	// li r3, 0
	std::pair { NameTag_Think+0xA4,            0x38600000u },
#else
	std::pair { NameTag_Think+0x60,            NOP },
	std::pair { NameTag_Think+0x80,            NOP },
	// li r3, 0
	std::pair { NameTag_Think+0x88,            0x38600000u },
#endif
	// Enable damage staling in develop mode
	// b 0x8
	std::pair { Damage_DamageStale+0x28,       0x48000008u },
#ifdef PAL
	// Skip save data creation prompt
	// li r4, SdMsgBox_CreateData
	std::pair { MessageBox_CheckData+0x110,    0x38600009u },
	// li r3, MsgBoxPrompt_None
	std::pair { MessageBox_CheckData+0x11C,    0x38600000u },
	// li r4, SdMsgBox_CreateData
	std::pair { MessageBox_CheckData+0x120,    0x38800009u },
	// li r0, MsgBox_CreateData
	std::pair { MessageBox_CheckData+0x128,    0x38000006u },
#else
	// Skip save data creation prompt
	// li r3, MsgBoxPrompt_None
	std::pair { MessageBox_CheckData+0x15C,    0x38600000u },
	// li r4, SdMsgBox_CreateData
	std::pair { MessageBox_CheckData+0x160,    0x3880000Bu },
	// li r0, MsgBox_CreateData
	std::pair { MessageBox_CheckData+0x168,    0x38000006u },
#endif
#if 0
	// Clone characters slide out from behind CSS
	std::pair { &CSSAnimStartFrame,            0.0 },
#endif
	// LRAStart at menu goes back to CSS
	// li r3, Scene_VsMode
	std::pair { SSS_Think+0x6C,                0x38600002u },
	// li r3, Scene_VsMode
	std::pair { CSS_Input+0x1F0,               0x38600002u },
	// Return to VS menu like normal after exiting CSS
	std::pair { MainMenu_Think+0x34,           NOP },
	// li r0, Scene_VsMode
	std::pair { MainMenu_Think+0x54,           0x38000002u },

	// Disable attract mode
	// b 0x18
	std::pair { TitleScreen_Think+0x48,        0x48000018u }
);
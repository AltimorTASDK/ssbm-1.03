#include "hsd/gobj.h"
#include "melee/player.h"
#include "melee/match.h"
#include "util/patch_list.h"
#include <gctypes.h>
#include <utility>

extern "C" void Camera_CStickControl();

extern "C" void CSS_PlayerThink(HSD_GObj *gobj);
extern "C" void CSS_Setup();
extern "C" void CSS_ReadyThink(HSD_GObj *gobj);
extern "C" void CSS_UpdatePortrait(u8 port);
extern "C" void CSS_GObjCallback(u8 port);

extern "C" void CreateKillScorePopup(u8 port);
extern "C" void CreateSDScorePopup(u8 port);

extern "C" void NameTag_Think(HSD_GObj *gobj);

extern "C" void VsMenu_Think(HSD_GObj *gobj);

extern "C" double CSSAnimStartFrame;

constexpr u32 NOP = 0x60000000;
constexpr u32 BLR = 0x4E800020;

static const auto patches = patch_list {
	// Enable cstick in develop mode
	std::pair { (char*)PlayerThink_Input+0x188,  NOP },
	// Enable cstick in singleplayer
	std::pair { (char*)PlayerThink_Input+0x194,  NOP },
	// Disable cstick camera control
	std::pair { (char*)Camera_CStickControl,     BLR },
	// Close CSS port on unplug
	// li r6, SlotType_None
	std::pair { (char*)CSS_PlayerThink+0x35C,    0x38C00003u },
	// Allow setting any port to HMN at CSS
	// li r3, SlotType_Human
	std::pair { (char*)CSS_PlayerThink+0x183C,   0x38600000u },
	// Apply random CPU character when opening fake HMN port
	// cmpwi r3, SlotType_Human
	std::pair { (char*)CSS_PlayerThink+0x1864,   0x28030000u },
	// Disable handicap display in CSS
	// li r0, 0
	std::pair { (char*)CSS_Setup+0x256C,         0x38000000u },
	std::pair { (char*)CSS_UpdatePortrait+0x650, 0x38000000u },
	std::pair { (char*)CSS_UpdatePortrait+0x660, 0x38000000u },
	std::pair { (char*)CSS_UpdatePortrait+0x660, 0x38000000u },
	std::pair { (char*)CSS_UpdatePortrait+0x738, 0x38000000u },
	std::pair { (char*)CSS_UpdatePortrait+0x748, 0x38000000u },
	std::pair { (char*)CSS_UpdatePortrait+0x86C, 0x38000000u },
	std::pair { (char*)CSS_UpdatePortrait+0xAAC, 0x38000000u },
	std::pair { (char*)CSS_UpdatePortrait+0xACC, 0x38000000u },
	std::pair { (char*)CSS_UpdatePortrait+0xB0C, 0x38000000u },
	std::pair { (char*)CSS_UpdatePortrait+0xBF0, 0x38000000u },
	std::pair { (char*)CSS_GObjCallback+0x2B0,   0x38000000u },
	std::pair { (char*)CSS_GObjCallback+0x2CC,   0x38000000u },
	std::pair { (char*)CSS_GObjCallback+0x364,   0x38000000u },
	std::pair { (char*)CSS_GObjCallback+0x3F8,   0x38000000u },
	std::pair { (char*)CSS_PlayerThink+0xBA0,    0x38000000u },
	std::pair { (char*)CSS_PlayerThink+0x1980,   0x38000000u },
	std::pair { (char*)CSS_PlayerThink+0x1A4C,   0x38000000u },
	// Disable score popups
	std::pair { (char*)CreateKillScorePopup,     BLR },
	std::pair { (char*)CreateSDScorePopup,       BLR },
	// Remove in-game port tags (P1/P2 etc)
	std::pair { (char*)NameTag_Think+0x60,       NOP },
	std::pair { (char*)NameTag_Think+0x80,       NOP },
	// li r3, 0
	std::pair { (char*)NameTag_Think+0x88,       0x38600000u },
	// Replace tournament melee with debug menu
	// li r0, 6
	std::pair { (char*)VsMenu_Think+0xA4,        0x38000006u },
#if 0
	// Clone characters slide out from behind CSS
	std::pair { (char*)&CSSAnimStartFrame,       0.0 },
#endif
};
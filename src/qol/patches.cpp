#include "melee/player.h"
#include "melee/match.h"
#include <gctypes.h>
#include <utility>

extern "C" void Camera_CStickControl();

extern "C" void CSS_Think();
extern "C" void CSS_Setup();
extern "C" void CSS_UpdatePortrait(u8 port);
extern "C" void CSS_GObjCallback(u8 port);

extern "C" void CreateKillScorePopup(u8 port);
extern "C" void CreateSDScorePopup(u8 port);

extern "C" void DisplayCrashScreen(void *cpu_context);

extern "C" void NameTag_Think(HSD_GObj *gobj);

constexpr u32 NOP = 0x60000000;
constexpr u32 BLR = 0x4E800020;

const auto patches = std::array {
	// Enable cstick in develop mode
	std::pair { (char*)PlayerThink_Input+0x188,  NOP },
	// Enable cstick in singleplayer
	std::pair { (char*)PlayerThink_Input+0x194,  NOP },
	// Disable cstick camera control
	std::pair { (char*)Camera_CStickControl,     BLR },
	// Close CSS port on unplug
	// li r6, 3
	std::pair { (char*)CSS_Think+0x35C,          0x38C00003u },
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
	// Disable score popups
	std::pair { (char*)CreateKillScorePopup,     BLR },
	std::pair { (char*)CreateSDScorePopup,       BLR },
	// Remove in-game port tags (P1/P2 etc)
	std::pair { (char*)NameTag_Think+0x60,       NOP },
	std::pair { (char*)NameTag_Think+0x70,       NOP },
	std::pair { (char*)NameTag_Think+0x80,       NOP },
	// li r3, 0
	std::pair { (char*)NameTag_Think+0x88,       0x38600000u },
	// Skip crash screen input checks
	// b +0x220
	std::pair { (char*)DisplayCrashScreen+0x4C,  0x48000220u },
};

struct apply_patches {
	apply_patches()
	{
		for (const auto &patch : patches)
			*(u32*)patch.first = patch.second;
	}
} apply_patches;
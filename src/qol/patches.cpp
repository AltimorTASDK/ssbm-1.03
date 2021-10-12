#include "melee/player.h"
#include <gctypes.h>
#include <utility>

extern "C" void Camera_CStickControl();

extern "C" void CSS_Think();
extern "C" void CSS_Setup();
extern "C" void CSS_UpdatePortrait(u8 port);
extern "C" void CSS_GObjCallback(u8 port);

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
};

struct apply_patches {
	apply_patches()
	{
		for (const auto &patch : patches)
			*(u32*)patch.first = patch.second;
	}
} apply_patches;
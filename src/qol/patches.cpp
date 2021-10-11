#include "melee/player.h"
#include <gctypes.h>
#include <utility>

extern "C" void Camera_CStickControl();
extern "C" void CSS_Think();

constexpr u32 NOP = 0x60000000;
constexpr u32 BLR = 0x4E800020;

const auto patches = std::array {
	// Enable cstick in develop mode
	std::pair { (char*)PlayerThink_Input+0x188, NOP },
	// Enable cstick in singleplayer
	std::pair { (char*)PlayerThink_Input+0x194, NOP },
	// Disable cstick camera control
	std::pair { (char*)Camera_CStickControl,    BLR },
	// Close CSS port on unplug
	// li r6, 3
	std::pair { (char*)CSS_Think+0x35C,         0x38C00003u },
};

struct apply_patches {
	apply_patches()
	{
		for (const auto &patch : patches)
			*(u32*)patch.first = patch.second;
	}
} apply_patches;
#include "melee/player.h"
#include <gctypes.h>
#include <utility>

extern "C" void Camera_CStickControl();

constexpr auto NOP = 0x60000000;
constexpr auto BLR = 0x4E800020;

const auto patches = std::array {
	// Enable cstick in develop mode
	std::pair { (char*)PlayerThink_Input+0x188, NOP },
	// Enable cstick in singleplayer
	std::pair { (char*)PlayerThink_Input+0x194, NOP },
	// Disable cstick camera control
	std::pair { (char*)Camera_CStickControl,    BLR },
};

struct apply_patches {
	apply_patches()
	{
		for (const auto &patch : patches)
			*(u32*)patch.first = patch.second;
	}
} apply_patches;
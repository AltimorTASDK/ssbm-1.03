#include "controls/config.h"
#include "hsd/archive.h"
#include "hsd/pad.h"
#include "util/melee/pad.h"
#include "util/math.h"
#include <bit>
#include <numeric>
#include <gctypes.h>

extern "C" void orig_PADRead(PADStatus *status);
extern "C" void hook_PADRead(PADStatus *status)
{
	orig_PADRead(status);

	for (auto i = 0; i < 4; i++) {
		if (status[i].err != 0) {
			// Reset config if unplugged and not in-game
			if (GetMatchInfo()->match_over)
				controller_configs[i].reset(i);
			continue;
		}

		// Swap X/Y bit with Z bit
		if (const auto xy_bit = controller_configs[i].z_jump_bit; xy_bit != 0)
			status[i].buttons = bit_swap(status[i].buttons, xy_bit, ButtonBit_Z);
	}
}

[[gnu::constructor]] static void set_match_over()
{
	// Initialize this field so it can be used to track if in-game
	GetMatchInfo()->match_over = true;
}
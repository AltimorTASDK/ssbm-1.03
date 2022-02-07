#include "controls/config.h"
#include "hsd/archive.h"
#include "hsd/pad.h"
#include "util/melee/pad.h"
#include "util/math.h"
#include <numeric>
#include <gctypes.h>

// X/Y + Z for 1 second at CSS
constexpr auto REMAP_HOLD_TIME = 60;

static void apply_remap(int port)
{
	const auto &config = controller_configs[port];

	if (config.z_jump_bit == 0)
		return;

	// Swap X/Y bit with Z bit
	auto *status = &HSD_PadMasterStatus[port];
	status->buttons = bit_swap(status->buttons, config.z_jump_bit, __builtin_ctz(Button_Z));
}

extern "C" void process_remapping(int port);
extern "C" void process_remapping(int port)
{
	// Reset config if unplugged and not in-game
	if (HSD_PadMasterStatus[port].err != 0 && GetMatchInfo()->match_over) {
		controller_configs[port].reset(port);
		return;
	}

	apply_remap(port);
}

[[gnu::constructor]] static void set_match_over()
{
	// Initialize this field so it can be used to track if in-game
	GetMatchInfo()->match_over = true;
}
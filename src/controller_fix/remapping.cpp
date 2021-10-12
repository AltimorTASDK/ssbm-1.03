#include "hsd/archive.h"
#include "hsd/pad.h"
#include "util/melee/pad.h"
#include "util/math.h"
#include <numeric>
#include <gctypes.h>

constexpr auto XY_MASK = (Button_X | Button_Y);
constexpr auto LRZ_MASK = (Button_L | Button_R | Button_Z);

// X/Y + L/R/Z for 1 second at CSS
constexpr auto REMAP_HOLD_TIME = 60;

u32 remapping[4] = { 0 };
int remap_timer[4] = { 0 };

void apply_remap(int port)
{
	if (remapping[port] == 0)
		return;
		
	auto *status = &HSD_PadMasterStatus[port];
	const auto xy_remap = remapping[port] & XY_MASK;
	const auto lrz_remap = remapping[port] & LRZ_MASK;
	
	auto old_buttons = status->buttons;
	
	// Trigger remapped input on analog press
	// Suppress analog inputs from the remapped trigger
	if (lrz_remap & Button_L) {
		if (status->raw_analog_l > TRIGGER_DEADZONE)
			old_buttons |= Button_L;

		status->raw_analog_l = 0;
		status->analog_l = 0;
	} else if (lrz_remap & Button_R) {
		if (status->raw_analog_r > TRIGGER_DEADZONE)
			old_buttons |= Button_R;

		status->raw_analog_r = 0;
		status->analog_r = 0;
	}

	// Swap xy_remap bit with lrz_remap bit
	status->buttons &= ~remapping[port];
	status->buttons |= (old_buttons & xy_remap) != 0 ? lrz_remap : 0;
	status->buttons |= (old_buttons & lrz_remap) != 0 ? xy_remap : 0;
}

void configure_remap(int port)
{
	// In CSS if MnSlChr.dat is loaded
	if (MnSlChr == nullptr)
		return;

	const auto &status = HSD_PadMasterStatus[port];
		
	// Check if already remapped
	if (remapping[port] != 0)
		return;
		
	const auto xy_buttons = status.buttons & XY_MASK;
	const auto lrz_buttons = status.buttons & LRZ_MASK;

	// Must be holding exactly one of X/Y and L/R/Z
	if (!is_pow2(xy_buttons) || !is_pow2(lrz_buttons)) {
		remap_timer[port] = 0;
		return;
	}
	
	if (++remap_timer[port] < REMAP_HOLD_TIME)
		return;
		
	// Successfully remapped
	remap_timer[port] = 0;
	remapping[port] = xy_buttons | lrz_buttons;

	HSD_PadRumble(port, 0, 0, 60);
}

extern "C" void process_remapping(int port);
extern "C" void process_remapping(int port)
{
	// Reset if unplugged
	if (HSD_PadMasterStatus[port].err != 0) {
		remapping[port] = 0;
		remap_timer[port] = 0;
		return;
	}

	configure_remap(port);
	apply_remap(port);
}
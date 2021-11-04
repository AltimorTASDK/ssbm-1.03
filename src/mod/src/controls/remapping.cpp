#include "controls/config.h"
#include "hsd/archive.h"
#include "hsd/pad.h"
#include "util/melee/pad.h"
#include "util/math.h"
#include <numeric>
#include <gctypes.h>

// X/Y + Z for 1 second at CSS
constexpr auto REMAP_HOLD_TIME = 60;

int remap_timer[4] = { 0 };

void apply_remap(int port)
{
	const auto &config = controller_configs[port];
	
	if (config.z_jump_bit == 0)
		return;
		
	// Swap X/Y bit with Z bit
	auto *status = &HSD_PadMasterStatus[port];
	status->buttons = bit_swap(status->buttons, config.z_jump_bit, __builtin_ctz(Button_Z));
}

void configure_remap(int port)
{
	// In CSS if MnSlChr.dat is loaded
	if (MnSlChr == nullptr) {
		remap_timer[port] = 0;
		return;
	}
		
	auto *config = &controller_configs[port];
	
	// Check if already remapped
	if (config->z_jump_bit != 0)
		return;

	const auto buttons = HSD_PadMasterStatus[port].buttons;
		
	// Must be holding exactly X+Z or Y+Z
	if (buttons != (Button_X | Button_Z) && buttons != (Button_Y | Button_Z)) {
		remap_timer[port] = 0;
		return;
	}
	
	if (++remap_timer[port] < REMAP_HOLD_TIME)
		return;
		
	// Successfully remapped
	remap_timer[port] = 0;
	config->z_jump_bit = (u8)__builtin_ctz(buttons & (Button_X | Button_Y));

	HSD_PadRumble(port, 0, 0, 60);
}

extern "C" void process_remapping(int port);
extern "C" void process_remapping(int port)
{
	// Reset config if unplugged
	if (HSD_PadMasterStatus[port].err != 0) {
		controller_configs[port].reset();
		remap_timer[port] = 0;
		return;
	}

	configure_remap(port);
	apply_remap(port);
}
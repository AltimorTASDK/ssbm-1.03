#include "util/coords.h"
#include <cmath>
#include <gctypes.h>

extern "C" void orig_HSD_PadClamp(s8 *x, s8 *y, bool shift, s8 min, s8 max);
extern "C" void hook_HSD_PadClamp(s8 *x, s8 *y, bool shift, s8 min, s8 max)
{
	const auto abs_x = std::abs(*x);
	const auto abs_y = std::abs(*y);

	// Produce 1.0 cardinals when one axis is >= 80 and the other is in the deadzone
	if (abs_x >= max && abs_y <= deadzone) {
		*x = std::copysign(max, *x);
		*y = 0;
		return;
	} else if (abs_y >= max && abs_x <= deadzone) {
		*x = 0;
		*y = std::copysign(max, *y);
		return;
	}
	
	orig_HSD_PadClamp(x, y, shift, min, max);
}
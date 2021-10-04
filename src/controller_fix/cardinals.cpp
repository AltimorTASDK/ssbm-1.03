#include "util/coords.h"
#include <cmath>
#include <gctypes.h>

void fix_stick(const vec2c &hw_values, vec2 *stick)
{
	const auto abs_x = std::abs(hw_values.x);
	const auto abs_y = std::abs(hw_values.y);

	// Produce 1.0 cardinals when one axis is >= 80 and the other is in the deadzone
	if (abs_x >= STICK_MAX && abs_y <= DEADZONE) {
		stick->x = std::copysign(1.f, hw_values.x);
		stick->y = 0;
	} else if (abs_y >= STICK_MAX && abs_x <= DEADZONE) {
		stick->x = 0;
		stick->y = std::copysign(1.f, hw_values.y);
	}
}

extern "C" void apply_cardinal_fix(HSD_GObj *gobj)
{
	auto *player = gobj->get<Player>();
	const auto &pad = get_input(player->port, 0);

	fix_stick(pad.stick, &player->input.stick);
	fix_stick(pad.cstick, &player->input.cstick);
}
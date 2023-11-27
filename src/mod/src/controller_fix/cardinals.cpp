#include "rules/values.h"
#include "util/melee/pad.h"
#include <cmath>
#include <gctypes.h>

static void fix_stick(const vec2c &hw_values, vec2 *stick)
{
	const auto abs_x = std::abs(hw_values.x);
	const auto abs_y = std::abs(hw_values.y);

	// Produce 1.0 cardinals when one axis is >= 80 and the other is in the deadzone
	if (abs_x >= STICK_MAX && abs_y <= DEADZONE) {
		stick->x = hw_values.x > 0 ? 1.f : -1.f;
		stick->y = 0;
	} else if (abs_y >= STICK_MAX && abs_x <= DEADZONE) {
		stick->x = 0;
		stick->y = hw_values.y > 0 ? 1.f : -1.f;
	}
}

extern "C" void apply_cardinal_fix(HSD_GObj *gobj)
{
	if (get_cfix() < cfix::a)
		return;

	auto *player = gobj->get<Player>();

	if (Player_IsCPU(player))
		return;

	const auto &pad = get_input<0>(player->port);

	fix_stick(pad.stick, &player->input.stick);
	fix_stick(pad.cstick, &player->input.cstick);
}
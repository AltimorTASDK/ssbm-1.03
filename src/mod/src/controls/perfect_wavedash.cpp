#include "controls/config.h"
#include "melee/player.h"
#include "util/math.h"
#include "util/melee/pad.h"
#include <cmath>

extern "C" float orig_Player_GetAirdodgeAngle(Player *player);
extern "C" float hook_Player_GetAirdodgeAngle(Player *player)
{
	if (!get_player_config(player).perfect_wavedash)
		return orig_Player_GetAirdodgeAngle(player);
		
	const auto &pad = get_input<0>(player->port);

	// Perfect WD if the X axis is >= 80 and the Y axis is in the deadzone
	if (std::abs(pad.stick.x) < STICK_MAX || std::abs(pad.stick.y) > DEADZONE)
		return orig_Player_GetAirdodgeAngle(player);
		
	constexpr auto perfect_x = .9500f;
	constexpr auto perfect_y = .2875f;
		
	return std::atan2(-perfect_y, pad.stick.x > 0 ? perfect_x : -perfect_x);
}
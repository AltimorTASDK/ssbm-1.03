#ifndef TOURNAMENT

#include "controls/config.h"
#include "melee/player.h"
#include "util/math.h"
#include "util/melee/pad.h"
#include <cmath>

extern "C" float orig_Player_GetAirdodgeAngle(Player *player);
extern "C" float hook_Player_GetAirdodgeAngle(Player *player)
{
	if (!get_player_config(player).perfect_angles)
		return orig_Player_GetAirdodgeAngle(player);

	const auto &pad = get_character_input<0>(player);

	// Perfect WD if the X axis is >= 64 (.8000) and the Y axis is in the deadzone
	if (std::abs(pad.stick.x) < 64 || std::abs(pad.stick.y) > DEADZONE)
		return orig_Player_GetAirdodgeAngle(player);

	constexpr auto perfect_x =  .9500f;
	constexpr auto perfect_y = -.2875f;
	constexpr auto angle_pos = std::atan2(perfect_y, perfect_x);
	constexpr auto angle_neg = std::atan2(perfect_y, -perfect_x);

	// Use converted coordinates for Nana
	constexpr auto nana_x_pos = popo_to_nana_float(perfect_x);
	constexpr auto nana_x_neg = popo_to_nana_float(-perfect_x);
	constexpr auto nana_y = popo_to_nana_float(perfect_y);
	constexpr auto nana_angle_pos = std::atan2(nana_y, nana_x_pos);
	constexpr auto nana_angle_neg = std::atan2(nana_y, nana_x_neg);

	if (player->character_id == CID_Nana)
		return pad.stick.x > 0 ? nana_angle_pos : nana_angle_neg;
	else
		return pad.stick.x > 0 ? angle_pos : angle_neg;
}

#endif
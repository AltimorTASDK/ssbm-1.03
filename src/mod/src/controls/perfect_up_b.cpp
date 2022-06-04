#ifndef TOURNAMENT

#include "controls/config.h"
#include "hsd/gobj.h"
#include "hsd/pad.h"
#include "melee/player.h"
#include "util/melee/pad.h"
#include <type_traits>

static bool apply_perfect_up_b_coords(Player *player)
{
	if (!get_player_config(player).perfect_angles)
		return false;

	const auto &pad = HSD_PadMasterStatus[player->port];

	if (!is_rim_coord(pad.stick))
		return false;

	const auto abs_x = std::abs(pad.stick.x);
	const auto abs_y = std::abs(pad.stick.y);

	if (abs_x < .9875f && abs_y < .2875f) {
		player->input.stick = { pad.stick.x > 0 ? .9500f : -.9500f,
		                        pad.stick.y > 0 ? .2875f : -.2875f };
	} else if (abs_y < .9875f && abs_x < .2875f) {
		player->input.stick = { pad.stick.x > 0 ? .2875f : -.2875f,
		                        pad.stick.y > 0 ? .9500f : -.9500f };
	} else {
		return false;
	}

	return true;
}

static auto apply_perfect_up_b(HSD_GObj *gobj, auto &&original, auto &&...args)
{
	using return_type = decltype(original(gobj, std::forward<decltype(args)>(args)...));

	auto *player = gobj->get<Player>();
	const auto old_stick = player->input.stick;

	if (!apply_perfect_up_b_coords(player))
		return original(gobj, std::forward<decltype(args)>(args)...);

	if constexpr (std::is_same_v<return_type, void>) {
		original(gobj, std::forward<decltype(args)>(args)...);
		player->input.stick = old_stick;
	} else {
		const auto result = original(gobj, std::forward<decltype(args)>(args)...);
		player->input.stick = old_stick;
		return result;
	}
}

extern "C" void orig_AS_356_Spacie_UpBAirMiddle(HSD_GObj *gobj);
extern "C" void hook_AS_356_Spacie_UpBAirMiddle(HSD_GObj *gobj)
{
	apply_perfect_up_b(gobj, orig_AS_356_Spacie_UpBAirMiddle);
}

extern "C" void orig_AS_359_Sheik_UpBAirMiddle(HSD_GObj *gobj);
extern "C" void hook_AS_359_Sheik_UpBAirMiddle(HSD_GObj *gobj)
{
	apply_perfect_up_b(gobj, orig_AS_359_Sheik_UpBAirMiddle);
}

extern "C" void orig_AS_357_Pika_UpBAirMiddle(HSD_GObj *gobj);
extern "C" void hook_AS_357_Pika_UpBAirMiddle(HSD_GObj *gobj)
{
	apply_perfect_up_b(gobj, orig_AS_357_Pika_UpBAirMiddle);
}

extern "C" bool orig_Player_Pika_CheckDoubleUpB(HSD_GObj *gobj);
extern "C" bool hook_Player_Pika_CheckDoubleUpB(HSD_GObj *gobj)
{
	return apply_perfect_up_b(gobj, orig_Player_Pika_CheckDoubleUpB);
}

extern "C" void orig_AS_357_Mewtwo_UpBAirMiddle(HSD_GObj *gobj);
extern "C" void hook_AS_357_Mewtwo_UpBAirMiddle(HSD_GObj *gobj)
{
	apply_perfect_up_b(gobj, orig_AS_357_Mewtwo_UpBAirMiddle);
}

extern "C" void orig_AS_353_Zelda_UpBAirMiddle(HSD_GObj *gobj);
extern "C" void hook_AS_353_Zelda_UpBAirMiddle(HSD_GObj *gobj)
{
	apply_perfect_up_b(gobj, orig_AS_353_Zelda_UpBAirMiddle);
}

#ifdef YOSHI_PERFECT_ANGLES
extern "C" void orig_Player_Yoshi_GetEggVelocity(HSD_GObj *gobj, vec3 *velocity);
extern "C" void hook_Player_Yoshi_GetEggVelocity(HSD_GObj *gobj, vec3 *velocity)
{
	apply_perfect_up_b(gobj, orig_Player_Yoshi_GetEggVelocity, velocity);
}
#endif

#endif
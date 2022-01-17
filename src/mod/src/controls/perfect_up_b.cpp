#include "controls/config.h"
#include "hsd/gobj.h"
#include "hsd/pad.h"
#include "melee/player.h"
#include "util/melee/pad.h"

//#include "os/os.h"

static void apply_perfect_up_b_coords(Player *player)
{
	if (!get_player_config(player).perfect_angles)
		return;

	const auto &pad = HSD_PadMasterStatus[player->port];

	if (!is_rim_coord(pad.stick))
		return;

	const auto abs_x = std::abs(pad.stick.x);
	const auto abs_y = std::abs(pad.stick.y);

	if (abs_x < .9875f && abs_y < .2875f) {
		player->input.stick = { pad.stick.x > 0 ? .9500f : -.9500f,
		                        pad.stick.y > 0 ? .2875f : -.2875f };
	} else if (abs_y < .9875f && abs_x < .2875f) {
		player->input.stick = { pad.stick.x > 0 ? .2875f : -.2875f,
		                        pad.stick.y > 0 ? .9500f : -.9500f };
	}
}

static void apply_perfect_up_b(HSD_GObj *gobj, auto &&original)
{
	auto *player = gobj->get<Player>();
	const auto old_stick = player->input.stick;
	apply_perfect_up_b_coords(player);
	original(gobj);
	player->input.stick = old_stick;
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
	auto *player = gobj->get<Player>();
	const auto old_stick = player->input.stick;
	apply_perfect_up_b_coords(player);
	const auto result = orig_Player_Pika_CheckDoubleUpB(gobj);
	player->input.stick = old_stick;

	return result;
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

#if 0
extern "C" void orig_Player_Yoshi_GetEggVelocity(HSD_GObj *gobj, vec3 *velocity);
extern "C" void hook_Player_Yoshi_GetEggVelocity(HSD_GObj *gobj, vec3 *velocity)
{
	auto *player = gobj->get<Player>();
	const auto old_stick = player->input.stick;
	apply_perfect_up_b_coords(player);
	orig_Player_Yoshi_GetEggVelocity(gobj, velocity);
	player->input.stick = old_stick;
}
#endif
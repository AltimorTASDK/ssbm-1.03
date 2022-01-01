#ifdef PAL

#include "hsd/gobj.h"
#include "hsd/pad.h"
#include "melee/item.h"
#include "melee/player.h"

static bool check_extender_input(const Player *player)
{
	switch (player->char_data.samus.extender_state) {
		case 0:  return player->input.held_buttons & Button_DPadUp;
		case 1:  return player->input.held_buttons & Button_DPadDown;
		case 2:  return player->input.held_buttons & Button_DPadUp;
		case 3:  return player->input.instant_buttons & Button_A;
		default: return false;
	}
}

static void update_extender(HSD_GObj *gobj, auto &&original)
{
	auto *item = gobj->get<Item>();
	auto *player = item->owner->get<Player>();
	auto extender_state = player->char_data.samus.extender_state;

	if (check_extender_input(player))
		extender_state++;

	// Prevent state from being reset to 0
	original(gobj);
	player->char_data.samus.extender_state = extender_state;
}

extern "C" void orig_Item_GrappleBeam_Startup(HSD_GObj *gobj);
extern "C" void hook_Item_GrappleBeam_Startup(HSD_GObj *gobj)
{
	update_extender(gobj, orig_Item_GrappleBeam_Startup);
}

extern "C" void orig_Item_GrappleBeam_Active(HSD_GObj *gobj);
extern "C" void hook_Item_GrappleBeam_Active(HSD_GObj *gobj)
{
	update_extender(gobj, orig_Item_GrappleBeam_Active);
}

#endif
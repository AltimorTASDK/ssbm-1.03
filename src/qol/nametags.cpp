#include "melee/action_state.h"
#include "melee/player.h"
#include <gctypes.h>

extern "C" bool should_show_nametag(s32 slot)
{
	const auto *gobj = PlayerBlock_GetGObj(slot);

	if (gobj == nullptr)
		return true;

	const auto *player = gobj->get<Player>();
	
	if (player->character_id == CID_Mewtwo && player->action_state == AS_EscapeAir)
		return false;
	
	if (player->is_invisible)
		return false;
		
	return true;
}
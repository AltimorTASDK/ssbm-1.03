#include "hsd/gobj.h"
#include "melee/action_state.h"
#include "melee/player.h"
#include "util/patch_list.h"
#include <gctypes.h>
#include <utility>

extern "C" void NameTag_Think(HSD_GObj *gobj);

PATCH_LIST(
	// Remove in-game port tags (P1/P2 etc)
#ifdef PAL
	// nop
	std::pair { NameTag_Think+0x7C,            0x60000000 },
	// nop
	std::pair { NameTag_Think+0x9C,            0x60000000 },
	// li r3, 0
	std::pair { NameTag_Think+0xA4,            0x38600000u },
#else
	// nop
	std::pair { NameTag_Think+0x60,            0x60000000 },
	// nop
	std::pair { NameTag_Think+0x80,            0x60000000 },
	// li r3, 0
	std::pair { NameTag_Think+0x88,            0x38600000u }
#endif
);

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
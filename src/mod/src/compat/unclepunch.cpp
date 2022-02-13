#if defined(NTSC102) && !defined(NOPAL)
#include "hsd/gobj.h"
#include "melee/menu.h"
#include "melee/scene.h"
#include "util/patch_list.h"

extern "C" struct {
	u32 game;
	u16 company;
} __GameCode;

extern "C" void Interrupt_AS_DamageFall(HSD_GObj *gobj);
extern "C" void Interrupt_AS_Turn(HSD_GObj *gobj);
extern "C" void AS_235_SpotDodge_YoshiCheck(HSD_GObj *gobj);
extern "C" void CSS_Setup();
extern "C" void CSS_ReadyThink(HSD_GObj *gobj);
extern "C" void CSS_PlayerThink(HSD_GObj *gobj);
extern "C" void Scene_Initialize(SceneMinorData *data);
extern "C" void HSD_PadRenewMasterStatus();
extern "C" void Match_InitPlayers();
extern "C" void Player_Respawn(s32 slot, s32 subchar);

// Check for UP gamecode
static const auto result = __GameCode.game == 'GTME';

extern "C" bool is_unclepunch()
{
	return result;
}

[[gnu::constructor]] static void check_unclepunch()
{
	if (!is_unclepunch())
		return;

	patch_list {
		// Remove UP's built in UCF
		// stfs f0, 0x2C(r31)
		std::pair { (char*)Interrupt_AS_Turn+0x4C,           0xD01F002Cu },
		// lwz r4, 0x2C(r3)
		std::pair { (char*)AS_235_SpotDodge_YoshiCheck+0x10, 0x8083002Cu },
		// mr r4, r24
		std::pair { (char*)CSS_Setup+0x2230,                 0x38980000u },
		// cmpw r3, r0
		std::pair { (char*)Interrupt_AS_DamageFall+0xCC,     0x7C030000u },

		// Get rid of UP's 1p start code because it conflicts with 1.03's
		// cmpwi r4, 2
		std::pair { (char*)CSS_ReadyThink+0x120,             0x2C040002u },

		// Remove UP's built in neutral spawns
		// lbz r0, 0x24D0(r31)
		std::pair { (char*)Match_InitPlayers+0x254,          0x881F24D0u },
		// mr r3, r27
		std::pair { (char*)Player_Respawn+0x80,              0x387B0000u },

		// Increase CSS text heap size in older UP versions to prevent crashes at
		// OSD/menu music. Newer UP versions have this built in
		// li r3, 0x4800
		std::pair { (char*)Scene_Initialize+0x54,            0x38604800u },

		// Disable UP's built in rumble toggle
		std::pair { (char*)CSS_PlayerThink+0x638,            0x889F0004u },
		// Disable UP's built in rumble off on unplug
		std::pair { (char*)HSD_PadRenewMasterStatus+0x98,    0x8819000Au },
	};
}

// Check for the flag UnclePunch uses to indicate the OSD menu
extern "C" bool is_unclepunch_osd()
{
	return IsEnteringMenu >= 2;
}
#else
extern "C" bool is_unclepunch()
{
	return false;
}

extern "C" bool is_unclepunch_osd()
{
	return false;
}
#endif
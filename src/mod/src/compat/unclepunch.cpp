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
extern "C" void Scene_Initialize(SceneMinorData *data);

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

		// Increase CSS text heap size in older UP versions to prevent crashes at
		// OSD/menu music. Newer UP versions have this built in
		// li r3, 0x4800
		std::pair { (char*)Scene_Initialize+0x54,            0x38604800u },
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
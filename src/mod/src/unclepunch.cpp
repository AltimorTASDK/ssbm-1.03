#include "hsd/gobj.h"
#include "os/card.h"
#include "util/patch_list.h"

extern "C" void Interrupt_AS_DamageFall(HSD_GObj *gobj);
extern "C" void Interrupt_AS_Turn(HSD_GObj *gobj);
extern "C" void AS_235_SpotDodge_YoshiCheck(HSD_GObj *gobj);
extern "C" void CSS_Setup();

struct check_unclepunch
{
	check_unclepunch()
	{
		// Check for UP gamecode
		if (cardmap[0].gamecode->game != 'GTME')
			return;

		patch_list {
			// Remove UP's built in UCF
			// stfs f0, 0x2C(r31)
			std::pair { (char*)Interrupt_AS_Turn+0x50,           0xD01F002Cu },
			// lwz r4, 0x2C(r3)
			std::pair { (char*)AS_235_SpotDodge_YoshiCheck+0x10, 0x8083002Cu },
			// mr r4, r24
			std::pair { (char*)CSS_Setup+0x2230,                 0x38980000u },
			// cmpw r3, r0
			std::pair { (char*)Interrupt_AS_DamageFall+0xCC,     0x7C030000u },
		};
	}
} check_unclepunch;
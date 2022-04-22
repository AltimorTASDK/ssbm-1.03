#if defined(NTSC100) || defined(NTSC101)

#include "melee/action_state.h"
#include "util/patch_list.h"

extern "C" ActionStateInfo LinkActionStates[21];
extern "C" ActionStateInfo YLinkActionStates[21];

PATCH_LIST(
	// Disable chain dance :(
	std::pair { &LinkActionStates[19].stat_flags, 0x00200000u },
	std::pair { &LinkActionStates[20].stat_flags, 0x00C00000u },
	std::pair { &YLinkActionStates[19].stat_flags, 0x00200000u },
	std::pair { &YLinkActionStates[20].stat_flags, 0x00C00000u }
);

#endif
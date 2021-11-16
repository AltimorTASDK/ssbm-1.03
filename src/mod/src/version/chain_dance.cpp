#if defined(NTSC100) || defined(NTSC101)

#include "melee/action_state.h"
#include "util/patch_list.h"
#include <gctypes.h>

static const auto patches = patch_list {
	// Disable chain dance :(
	std::pair { &LinkActionStates[19].stat_flags, 0x00200000u },
	std::pair { &LinkActionStates[20].stat_flags, 0x00C00000u },
};

#endif
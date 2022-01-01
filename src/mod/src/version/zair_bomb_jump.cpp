#ifdef PAL

#include "melee/action_state.h"
#include "util/patch_list.h"

extern "C" ActionStateInfo SamusActionStates[18];

static const auto patches = patch_list {
	// Allow Samus to bomb jump out of zair in PAL
	std::pair { &SamusActionStates[16].stat_flags, 0x00200000u },
};

#endif
#ifdef DOL

#include "hsd/heap.h"
#include "melee/scene.h"
#include "util/patch_list.h"
#include <cstring>

extern "C" char SaveFileName[25];
extern "C" void BootScene_Exit(SceneMinorData *data);

extern "C" char __NEW_BASE__;

extern "C" char __BSS_START__;
extern "C" char __BSS_SIZE__;

PATCH_LIST(
	// Patch end of save file name to match the name normally set by the memcard loader
	std::pair { &SaveFileName[20],   '_103' },
	// Boot to CSS
	// li r3, Scene_VsMode
	std::pair { BootScene_Exit+0x78, 0x38600002u }
);

extern "C" void orig___init_data();
extern "C" void hook___init_data()
{
	orig___init_data();

	// Zero out the bussy
	memset(&__BSS_START__, 0, (size_t)&__BSS_SIZE__);
}

extern "C" void orig_OSSetArenaHi(void *hi);
extern "C" void hook_OSSetArenaHi(void *hi)
{
	// Prevent heap from overrunning mod code
	orig_OSSetArenaHi(&__NEW_BASE__);

	static constinit auto adjusted_heap = false;

	if (!adjusted_heap) {
		// Reduce size of grow-down heap so the base remains the same
		const auto delta = (u32)hi - (u32)&__NEW_BASE__;
		hsd_default_heaps[2].size -= delta;
		adjusted_heap = true;
	}
}

#endif
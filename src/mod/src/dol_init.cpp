#ifdef DOL

#include "melee/scene.h"
#include "util/patch_list.h"

extern "C" char SaveFileName[25];
extern "C" void BootScene_Exit(SceneMinorData *data);

PATCH_LIST(
	// Patch end of save file name to match the name normally set by the memcard loader
	std::pair { &SaveFileName[20],   '_103' },
	// Boot to CSS
	// li r3, Scene_VsMode
	std::pair { BootScene_Exit+0x78, 0x38600002u }
);

#endif
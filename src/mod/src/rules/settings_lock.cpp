#include "melee/scene.h"
#include "rules/settings_lock.h"
#include "util/patch_list.h"
#include <ogc/card.h>
#include <gctypes.h>

extern "C" void Scene_Execute(struct SceneMajorData *major);

static const auto patches = patch_list {
	// Go to CSS on reset
	// li r3, Scene_VsMode
	std::pair { (char*)Scene_Execute+0x244, 0x38600002u },
};

extern "C" void orig_InitializeSaveData(u32 status);
extern "C" void hook_InitializeSaveData(u32 status)
{
	// Prevent save data reset
}

extern "C" void hook_InitializeGlobalData();
extern "C" void hook_InitializeGlobalData()
{
	if (!CARD_Probe(CARD_SLOTA))
		settings_locked = true;
		
	// Prevent global data reset
}
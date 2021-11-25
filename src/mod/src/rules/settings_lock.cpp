#include "melee/scene.h"
#include "rules/values.h"
#include "util/patch_list.h"
#include <ogc/card.h>
#include <gctypes.h>

extern "C" void Scene_Execute(struct SceneMajorData *major);

static const auto patches = patch_list {
	// Go to CSS on reset
	// li r3, Scene_VsMode
	std::pair { (char*)Scene_Execute+0x244, 0x38600002u },
};

static bool settings_lock = false;

bool get_settings_lock()
{
	return settings_lock;
}

extern "C" void orig_InitializeSaveData(u32 status);
extern "C" void hook_InitializeSaveData(u32 status)
{
	// Prevent save data reset
}

extern "C" void hook_InitializeGlobalData();
extern "C" void hook_InitializeGlobalData()
{
	if (!CARD_Probe(CARD_SLOTA))
		settings_lock = true;

	// Prevent global data reset
}
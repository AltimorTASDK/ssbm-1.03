#include "controls/config.h"
#include "melee/music.h"
#include "melee/scene.h"
#include "rules/saved_config.h"
#include "rules/values.h"
#include "util/patch_list.h"
#include <ogc/card.h>
#include <gctypes.h>
#include <type_traits>

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
	// Prevent global data reset and check settings lock
	if (settings_lock || CARD_Probe(CARD_SLOTA))
		return;

	settings_lock = true;

	for (auto &controls : controller_configs)
		controls.make_legal();

	for (size_t i = 0; i < std::extent_v<decltype(config.stage_bgm)>; i++)
		config.stage_bgm[i] = BGM_Undefined;

	auto *rules = GetGameRules();

	rules->mode        = Mode_Stock;
	rules->time_limit  = 0;
	rules->stock_count = 4;
	rules->pause       = false;
}
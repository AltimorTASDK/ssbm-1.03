#include "compat/20XX.h"
#include "controls/config.h"
#include "melee/debug.h"
#include "melee/music.h"
#include "melee/scene.h"
#include "rules/saved_config.h"
#include "rules/values.h"
#include "util/patch_list.h"
#include <ogc/card.h>
#include <gctypes.h>
#include <type_traits>

extern "C" void Scene_Execute(struct SceneMajorData *major);

#ifndef PAL
PATCH_LIST(
	// Go to CSS on reset
	// li r3, Scene_VsMode
	std::pair { Scene_Execute+0x244, 0x38600002u }
);
#else
extern "C" void orig_Scene_Reset();
extern "C" void hook_Scene_Reset()
{
	// Go to CSS on reset
	orig_Scene_Reset();
	Scene_SetMajorPending(Scene_VsMode);
}
#endif

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

extern "C" void orig_InitializeGlobalData();
extern "C" void hook_InitializeGlobalData()
{
#ifdef DOL
	// Allow first InitializeGlobalData on boot
	static constinit auto initialized = false;

	if (!initialized) {
		initialized = true;
		return orig_InitializeGlobalData();
	}
#endif
#ifndef STEALTH
	// Prevent global data reset and check settings lock
	if (settings_lock || CARD_Probe(CARD_SLOTA))
		return;

	settings_lock = true;

	for (auto i = 0uz; i < std::extent_v<decltype(config.stage_bgm)>; i++)
		config.stage_bgm[i] = BGM_Undefined;

	auto *rules = GetGameRules();

	rules->mode        = Mode_Stock;
	rules->time_limit  = 0;
	rules->stock_count = 4;
	rules->pause       = false;

	if (!is_20XX()) {
		// Reset debug settings
		*DebugMenuEntries[DebugEntry_DbLevel].value     = DbLKind_Master;
		*DebugMenuEntries[DebugEntry_IKDebugFlag].value = true;
	}
#endif
}
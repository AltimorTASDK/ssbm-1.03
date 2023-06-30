#include "compat/unclepunch.h"
#include "melee/player.h"
#include "melee/preferences.h"
#include <gctypes.h>

extern "C" u8 FeatureUnlockMask;

extern "C" void UnlockAllCharacters();
extern "C" void UnlockAllStages();
extern "C" void UnlockAllFeatures();
extern "C" void UnlockAllTrophies();

extern "C" void SetDeflickerSetting(bool deflicker);

static void unlock_everything()
{
	FeatureUnlockMask = 0xFF;
	UnlockAllCharacters();
	UnlockAllStages();
	UnlockAllFeatures();
	UnlockAllTrophies();
}

extern "C" u32 hook_IsFeatureUnlocked(u32 index)
{
	return 1;
}

extern "C" u32 hook_IsTrophyUnlocked(u32 index)
{
	return 1;
}

extern "C" u32 orig_GetUnlockedEventMatchCount(u32 index);
extern "C" u32 hook_GetUnlockedEventMatchCount(u32 index)
{
	return !is_unclepunch() ? 42 : orig_GetUnlockedEventMatchCount(index);
}

extern "C" bool hook_ShouldShowUnlockMessage(u32 index)
{
	return false;
}

extern "C" u32 orig_MemoryCard_DoLoadData();
extern "C" u32 hook_MemoryCard_DoLoadData()
{
	// Unlock everything and always return success
	orig_MemoryCard_DoLoadData();
	unlock_everything();

	// Force rumble off regardless of saved settings
	for (auto port = 0; port < 4; port++)
		SetPortRumbleFlag(port, false);

	// Force deflicker off
	SetDeflickerSetting(false);

	// Apply settings from new save file
	ApplySoundBalance();
	ApplyVideoSettings();

	return 0;
}
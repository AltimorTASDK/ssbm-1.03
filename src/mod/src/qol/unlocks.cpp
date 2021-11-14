#include <gctypes.h>

extern "C" u8 FeatureUnlockMask;

extern "C" void UnlockAllCharacters();
extern "C" void UnlockAllStages();
extern "C" void UnlockAllFeatures();
extern "C" void UnlockAllTrophies();

static void unlock_everything()
{
	FeatureUnlockMask = 0xFF;
	UnlockAllCharacters();
	UnlockAllStages();
	UnlockAllFeatures();
	UnlockAllTrophies();
}

extern "C" u32 orig_MemoryCard_DoLoadData();
extern "C" u32 hook_MemoryCard_DoLoadData()
{
	// Unlock everything and always return success
	orig_MemoryCard_DoLoadData();
	unlock_everything();
	return 0;
}
#include <gctypes.h>

extern "C" u8 FeatureUnlockMask;

extern "C" void UnlockAllCharacters();
extern "C" void UnlockAllStages();
extern "C" void UnlockAllFeatures();
extern "C" void UnlockAllTrophies();

struct unlock_all {
	unlock_all()
	{
		FeatureUnlockMask = 0xFF;
		UnlockAllCharacters();
		UnlockAllStages();
		UnlockAllFeatures();
		UnlockAllTrophies();
	}
} unlock_all;
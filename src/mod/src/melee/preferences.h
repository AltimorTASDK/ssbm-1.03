#pragma once

#include <gctypes.h>

enum GmLangType {
	GmLangTypeJP = 0,
	GmLangTypeUS = 1,
#ifdef PAL
	GmLangTypeUK = 2,
	GmLangTypeGM = 3,
	GmLangTypeFR = 4,
	GmLangTypeIT = 5,
	GmLangTypeSP = 6
#endif
};

struct SavedPreferences {
	char pad000[0x16];
	u8 language;
	u32 random_stage_mask;
	char pad01C[0x20 - 0x1C];
};

extern "C" {

SavedPreferences *GetSavedPreferences();
bool IsLanguageUS();

void ApplySoundBalance();
void ApplyVideoSettings();

} // extern "C"
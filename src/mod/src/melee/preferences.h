#pragma once

#include <gctypes.h>

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
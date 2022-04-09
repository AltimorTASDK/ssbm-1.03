#pragma once

#include <gctypes.h>

struct SavedPreferences {
	char pad000[0x16];
	u8 language;
	char pad017[0x20 - 0x17];
};

extern "C" {

SavedPreferences *GetSavedPreferences();
bool IsLanguageUS();

} // extern "C"
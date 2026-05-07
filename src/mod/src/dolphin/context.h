#pragma once

#include <gctypes.h>

struct OSContext {
	u32 gpr[32];
	char pad080[0x2C8 - 0x80];
};

extern "C" {

void OSSaveFPUContext(OSContext *context);
void OSLoadFPUContext(OSContext *context);

} // extern "C"
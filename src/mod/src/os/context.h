#pragma once

struct OSContext {
	char pad000[0x2C8];
};

extern "C" {
void OSSaveFPUContext(OSContext *context);
void OSLoadFPUContext(OSContext *context);
}
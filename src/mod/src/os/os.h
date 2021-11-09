#pragma once

#include <gctypes.h>

#define PANIC(...) OSPanic(__FILE__, __LINE__, __VA_ARGS__)

extern "C" {

[[noreturn]] void OSPanic(const char *file, int line, const char *fmt, ...);
void OSReport(const char *fmt, ...);
u64 OSGetTime();

u32 OSDisableInterrupts();
void OSRestoreInterrupts(u32 mask);

}
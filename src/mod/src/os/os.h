#pragma once

#include <gctypes.h>
#include <cstddef>

#define HEAP_DEFAULT (-1)

#define PANIC(...) OSPanic(__FILE__, __LINE__, __VA_ARGS__)

extern "C" {

__attribute__((noreturn)) void OSPanic(const char *file, int line, const char *fmt, ...);
void OSReport(const char *fmt, ...);
void *OSAllocFromHeap(int heap, size_t size);
u64 __OSGetSystemTime();

u32 OSDisableInterrupts();
void OSRestoreInterrupts(u32 mask);

}
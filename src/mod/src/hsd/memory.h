#pragma once

#include <gctypes.h>

struct HSD_AllocEntry {
	HSD_AllocEntry *next;
	void *addr;
	u32 size;
};

extern "C" {

void *HSD_MemAlloc(u32 size);
void *HSD_MemAllocFromHeap(u32 heap, u32 size);
void HSD_Free(void *ptr);
void HSD_FreeToHeap(u32 heap, void *ptr);

}
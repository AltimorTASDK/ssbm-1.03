#pragma once

#include <cstddef>

extern "C" {

void *HSD_MemAlloc(size_t size);
void *HSD_MemAllocFromHeap(int heap, size_t size);
void HSD_Free(void *ptr);
void HSD_FreeToHeap(int heap, void *ptr);

}
#pragma once

#include <gctypes.h>

struct OSHeapChunk {
	OSHeapChunk *prev;
	OSHeapChunk *next;
	s32 size;
};

struct OSHeap {
	s32 size;
	OSHeapChunk *free_list;
	OSHeapChunk *reserved_list;
};

extern "C" {

extern OSHeap *HeapList;
extern s32 MaxHeapCount;

s32 OSCreateHeap(void *start, void *end);
void OSDestroyHeap(s32 heap);
void *OSAllocFromHeap(s32 heap, u32 size);
void OSFreeToHeap(s32 heap, void *buf);

} // extern "C"
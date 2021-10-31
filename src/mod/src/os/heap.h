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

void *OSAllocFromHeap(int heap, u32 size);

}
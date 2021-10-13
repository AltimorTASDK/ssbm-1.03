#include <gctypes.h>
#include "os/os.h"

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

extern "C" char __LOAD_BASE__;
constexpr void *load_base = &__LOAD_BASE__;

extern "C" OSHeap *HeapList;
extern "C" s32 MaxHeapCount;
extern "C" void *hsd_heap_arena_hi;

void fix_heap(OSHeap *heap)
{
	// Resize the heap to avoid overwriting mod code
	auto *chunk = heap->free_list;

	while (chunk != nullptr) {
		const auto *chunk_end = (char*)chunk + chunk->size;

		if (chunk >= load_base) {
			// Chunk is entirely within code space, unlink
			if (chunk->prev != nullptr)
				chunk->prev->next = chunk->next;
			else
				heap->free_list = chunk->next;
				
			if (chunk->next != nullptr)
				chunk->next->prev = chunk->prev;
				
			break;
		} else if (chunk_end >= load_base) {
			// Chunk is partially within code space, reduce size
			chunk->size = (s32)load_base - (s32)chunk;
		}

		chunk = chunk->next;
	}
}

struct manage_heaps {
	manage_heaps()
	{
		for (auto i = 0; i < MaxHeapCount; i++) {
			auto *heap = &HeapList[i];
			if (heap->size >= 0)
				fix_heap(heap);
		}
		
		if (hsd_heap_arena_hi > load_base)
			hsd_heap_arena_hi = load_base;
	}
} manage_heaps;
#pragma once

#include <gctypes.h>

struct HSD_HeapHandle;

struct HSD_Heap {
	s32 id;
	HSD_HeapHandle *handle;
	void *start;
	u32 size;
	u32 type;
	u32 pad014;
	u32 deleted;
};

extern "C" {

extern void *hsd_heap_arena_lo;
extern void *hsd_heap_arena_hi;
extern void *default_hsd_heap_arena_lo;
extern void *default_hsd_heap_arena_hi;
extern HSD_Heap hsd_heap_list[6];

} // extern "C"
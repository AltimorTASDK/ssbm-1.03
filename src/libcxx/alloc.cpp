#include "hsd/memory.h"
#include <new>

// Explicitly use heap 0 because it doesn't get destroyed on scene transition
constexpr auto heap = 0;

void *operator new(size_t count)
{
	return HSD_MemAllocFromHeap(heap, count);
}

void *operator new[](size_t count)
{
	return HSD_MemAllocFromHeap(heap, count);
}

void operator delete(void *ptr)
{
	HSD_FreeToHeap(heap, ptr);
}

void operator delete(void *ptr, size_t sz)
{
	HSD_FreeToHeap(heap, ptr);
}

void operator delete[](void *ptr)
{
	HSD_FreeToHeap(heap, ptr);
}

void operator delete[](void *ptr, size_t sz)
{
	HSD_FreeToHeap(heap, ptr);
}
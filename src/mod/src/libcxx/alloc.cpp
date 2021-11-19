#include "hsd/memory.h"
#include <cstddef>
#include <new>

void *operator new(size_t count)
{
	return HSD_MemAlloc(count);
}

void *operator new[](size_t count)
{
	return HSD_MemAlloc(count);
}

void operator delete(void *ptr)
{
	HSD_Free(ptr);
}

void operator delete(void *ptr, size_t sz)
{
	HSD_Free(ptr);
}

void operator delete[](void *ptr)
{
	HSD_Free(ptr);
}

void operator delete[](void *ptr, size_t sz)
{
	HSD_Free(ptr);
}
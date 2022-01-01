#include "util/mempool.h"
#include "util/gc/memcard.h"

mempool *mempool::pool_list_head = nullptr;

extern "C" void orig_HSD_ResetScene();
extern "C" void hook_HSD_ResetScene()
{
	// Wait for memcard operations to complete so the buffers don't get freed
	card_sync();

	// Run destructors and clear pool refcounts on heap destruction
	mempool::free_all();

	orig_HSD_ResetScene();
}
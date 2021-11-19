#if defined(NTSC100) || defined(PAL)

#include "hsd/gobj.h"
#include "hsd/memory.h"
#include "melee/player.h"
#include "os/dvd.h"
#include "util/diff.h"
#include <cstring>
#include <gctypes.h>

#ifdef NTSC100
#include "resources/patches/ntsc100/PlGn.diff.h"
#endif

struct PreloadEntry {
	u8 used;
	u8 type;
	u8 heap;
	u8 load_state;
	u8 pad004;
	u16 entry_num;
	u16 pad008;
	u32 size;
	HSD_AllocEntry *raw_data;
	HSD_AllocEntry *archive;
	u32 effect_index;
};

struct patch_entry {
	u32 entry_num;
	const void *diff;
};

const patch_entry patch_table[] = {
#ifdef NTSC100
	{ DVDConvertPathToEntrynum("PlGn.dat"), PlGn_diff_data },
#endif
};

extern "C" void patch_preloaded_archive(PreloadEntry *entry)
{
	for (const auto &patch : patch_table) {
		if (patch.entry_num != entry->entry_num)
			continue;

		const auto size = apply_diff(entry->raw_data->addr, patch.diff, nullptr);

		auto *out = (HSD_AllocEntry*)HSD_MemAllocFromHeap(entry->heap, size);
		apply_diff(entry->raw_data->addr, patch.diff, out->addr);

		// Replace original buffer
		HSD_FreeToHeap(entry->heap, entry->raw_data->addr);
		entry->size = size;
		entry->raw_data = out;
		return;
	}
}

#endif
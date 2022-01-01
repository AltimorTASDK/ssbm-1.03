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

#ifdef PAL
#include "resources/patches/pal/IfAll.diff.h"
#include "resources/patches/pal/PlCa.diff.h"
#include "resources/patches/pal/PlCl.diff.h"
#include "resources/patches/pal/PlFc.diff.h"
#include "resources/patches/pal/PlFx.diff.h"
#include "resources/patches/pal/PlGn.diff.h"
#include "resources/patches/pal/PlKb.diff.h"
#include "resources/patches/pal/PlKp.diff.h"
#include "resources/patches/pal/PlLk.diff.h"
#include "resources/patches/pal/PlMr.diff.h"
#include "resources/patches/pal/PlMs.diff.h"
#include "resources/patches/pal/PlMt.diff.h"
#include "resources/patches/pal/PlNs.diff.h"
#include "resources/patches/pal/PlPk.diff.h"
#include "resources/patches/pal/PlSk.diff.h"
#include "resources/patches/pal/PlYs.diff.h"
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
#ifdef PAL
	{ DVDConvertPathToEntrynum("IfAll.ukd"), IfAll_diff_data },
	{ DVDConvertPathToEntrynum("PlCa.dat"), PlCa_diff_data },
	{ DVDConvertPathToEntrynum("PlCl.dat"), PlCl_diff_data },
	{ DVDConvertPathToEntrynum("PlFc.dat"), PlFc_diff_data },
	{ DVDConvertPathToEntrynum("PlFx.dat"), PlFx_diff_data },
	{ DVDConvertPathToEntrynum("PlGn.dat"), PlGn_diff_data },
	{ DVDConvertPathToEntrynum("PlKb.dat"), PlKb_diff_data },
	{ DVDConvertPathToEntrynum("PlKp.dat"), PlKp_diff_data },
	{ DVDConvertPathToEntrynum("PlLk.dat"), PlLk_diff_data },
	{ DVDConvertPathToEntrynum("PlMr.dat"), PlMr_diff_data },
	{ DVDConvertPathToEntrynum("PlMs.dat"), PlMs_diff_data },
	{ DVDConvertPathToEntrynum("PlMt.dat"), PlMt_diff_data },
	{ DVDConvertPathToEntrynum("PlNs.dat"), PlNs_diff_data },
	{ DVDConvertPathToEntrynum("PlPk.dat"), PlPk_diff_data },
	{ DVDConvertPathToEntrynum("PlSk.dat"), PlSk_diff_data },
	{ DVDConvertPathToEntrynum("PlYs.dat"), PlYs_diff_data },
#endif
};

extern "C" void patch_preloaded_archive(PreloadEntry *entry)
{
	for (const auto &patch : patch_table) {
		if (patch.entry_num != entry->entry_num)
			continue;

		const auto size = apply_diff(entry->raw_data->addr, patch.diff, nullptr);

		// Move original buffer onto main heap to avoid OOM on preload heap
		auto *original = new char[entry->size];
		memcpy(original, entry->raw_data->addr, entry->size);
		HSD_FreeToHeap(entry->heap, entry->raw_data->addr);

		auto *out = (HSD_AllocEntry*)HSD_MemAllocFromHeap(entry->heap, size);
		apply_diff(original, patch.diff, out->addr);

		delete[] original;

		entry->size = size;
		entry->raw_data = out;
		return;
	}
}

#endif
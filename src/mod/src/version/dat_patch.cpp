#if defined(NTSC100) || defined(PAL)

#include "hsd/gobj.h"
#include "hsd/memory.h"
#include "melee/player.h"
#include "util/diff.h"
#include <array>
#include <cstring>
#include <gctypes.h>
#include <string>
#include <tuple>

//#include "resources/patches/ntsc100/PlGn.diff.h"

/*const auto patch_table = std::array {
#ifdef NTSC100
	std::pair { std::string("PlGn.dat"), PlGn_diff_data },
#endif
};

extern "C" void *orig_File_GetPreloaded(const char *filename);
extern "C" void *hook_File_GetPreloaded(const char *filename)
{
	auto *buf = orig_File_GetPreloaded(filename);

	if (buf == nullptr)
		return nullptr;

	for (const auto &pair : patch_table) {
		if (strcmp(pair.first.c_str(), filename) != 0)
			continue;

		OSReport("PlGn_diff_data %p\n", PlGn_diff_data);
		OSReport("*PlGn_diff_data %02X\n", *(char*)PlGn_diff_data);
		OSReport("pair.second %p\n", pair.second);
		OSReport("*pair.second %02X\n", *(char*)pair.second);
		const auto size = apply_diff((char*)buf, (char*)pair.second, nullptr);
		OSReport("size %08X\n", size);
		OSReport("buf %p\n", buf);
		auto *out = HSD_MemAlloc(size);
		apply_diff((char*)buf, (char*)pair.second, (char*)out);
		memcpy(buf, out, size);
		HSD_Free(out);
		return buf;
	}

	return buf;
}*/

extern "C" void orig_Player_InitializeFromInfo(HSD_GObj *gobj, void *info);
extern "C" void hook_Player_InitializeFromInfo(HSD_GObj *gobj, void *info)
{
	orig_Player_InitializeFromInfo(gobj, info);

	auto *player = gobj->get<Player>();
	char *buf = (char*)player->character_data->archive_base - 0x20;

#ifdef NTSC100
	if (player->character_id == CID_Ganondorf) {
		*(u32*)((char*)buf + 0x4E24) = 0x08000014;
		*(u16*)((char*)buf + 0x4E3E) = 0x03E8;
		*(u16*)((char*)buf + 0x4E52) = 0x02BC;
	}
#endif
}

#endif
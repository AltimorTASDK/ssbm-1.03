#include "hsd/memory.h"
#include "melee/nametag.h"
#include "melee/scene.h"
#include "os/card.h"
#include "os/os.h"
#include "os/thread.h"
#include "util/diff.h"
#include "util/gc/wait_object.h"
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <gctypes.h>
#include <ogc/cache.h>
#include <ogc/card.h>
#include <zlib.h>

extern "C" {

extern void *CardWorkArea;

extern char SaveFileName[25];

extern char __MOD_BASE__;

extern u8 CSSPendingSceneChange;

void OSEnableScheduler();

s32 OSSaveContext(OSContext *context);
void OSDefaultErrorHandler(OSContext *context);
s32 DisplayCrashScreen(OSContext *context);

void InitCardBuffers();

u32 MemoryCard_DoLoadData();
void ApplySoundBalance();

}

static card_file file;
static card_stat stats;
static wait_object wait;
static void *read_dest;
static const char *read_file;

extern "C" char __BSS_START__;
extern "C" char __BSS_SIZE__;

[[gnu::noreturn]] static void panic(const char *fmt, auto &&...args)
{
	OSReport(fmt, args...);

	// Enable interrupts so VI interrupts happen
	OSRestoreInterrupts(1);

	// Enable scheduler so crash screen thread runs
	OSEnableScheduler();

	OSContext context;
	OSSaveContext(&context);
	OSDefaultErrorHandler(&context);

	while (true);
}

static void card_callback(s32 chan, s32 result)
{
	if (result < 0)
		panic("Card read failed (%d)\n", result);

	// Wake up the main thread
	wait.wake();
}

static void read_callback(s32 chan, s32 result)
{
	if (result < 0)
		panic("Card mount failed (%d)\n", result);

	if (s32 error = CARD_Open(chan, read_file, &file); error < 0)
		panic("CARD_Open failed (%d)\n", error);

	if (s32 error = CARD_GetStatus(chan, file.filenum, &stats); error < 0)
		panic("CARD_GetStatus failed (%d)\n", error);

	if (read_dest == nullptr) {
		// Wake up the main thread
		wait.wake();
		return;
	}

	if (s32 error = CARD_ReadAsync(&file, read_dest, stats.len, 0, card_callback); error < 0)
		panic("CARD_ReadAsync failed (%d)\n", error);
}

static void patch_crash_screen()
{
	// Skip crash screen input checks
	// b +0x220
	auto *patch_location = (char*)DisplayCrashScreen+0x4C;
	*(u32*)patch_location = 0x48000220u;

	DCStoreRange(patch_location, 4);
	ICInvalidateRange(patch_location, 4);
}

static u32 card_read(const char *file, void *dest)
{
	read_file = file;
	read_dest = dest;

	wait.reset();

	if (s32 error = CARD_MountAsync(0, CardWorkArea, nullptr, read_callback); error < 0)
		panic("CARD_MountAsync failed (%d)\n", error);

	// Wait for read to complete
	wait.sleep();

	CARD_Unmount(0);

	return stats.len;
}

#ifndef NTSC102
static void *alloc_and_read(const char *file)
{
	const auto size = card_read(file, nullptr);
	auto *buf = HSD_MemAlloc(size);
	card_read(file, buf);
	return buf;
}
#endif

struct file_entry {
	u32 compressed_len;
	u32 uncompressed_len;
	char data[];
};

const file_entry *get_file(const void *data, int index)
{
	constexpr auto title_size = 0x40;
	const auto *ptr = (const char*)data + title_size;

	for (auto i = 0; i < index; i++)
		ptr += sizeof(file_entry) + ((const file_entry*)ptr)->compressed_len;

	const auto *entry = (file_entry*)ptr;

	if (entry->compressed_len == 0) {
#if defined(NTSC102)
		panic("This build of 1.03 does not support NTSC 1.02. lmao\n");
#elif defined(NTSC101)
		panic("This build of 1.03 does not support NTSC 1.01.\n");
#elif defined(NTSC100)
		panic("This build of 1.03 does not support NTSC 1.00.\n");
#elif defined(PAL)
		panic("This build of 1.03 does not support PAL.\n");
#endif
	}

	return entry;
}

extern "C" void *zcalloc(void *opaque, u32 items, u32 size)
{
	return HSD_MemAlloc(items * size);
}

extern "C" void zcfree(void *opaque, void *ptr)
{
	return HSD_Free(ptr);
}

static void decompress(const void *in, size_t in_size, void *out, size_t out_size)
{
	z_stream stream = {
		.zalloc = zcalloc,
		.zfree  = zcfree
	};

	if (const auto err = inflateInit(&stream); err != Z_OK)
		panic("inflateInit failed: %d\n", err);

	stream.next_in = (const Byte*)in;
	stream.avail_in = in_size;
	stream.next_out = (Byte*)out;
	stream.avail_out = out_size;

	if (const auto err = inflate(&stream, Z_FINISH); err != Z_STREAM_END)
		panic("inflate failed: %d\n", err);

	if (const auto err = inflateEnd(&stream); err != Z_OK)
		panic("inflate failed: %d\n", err);
}

extern "C" [[gnu::section(".loader")]] void load_mod()
{
	OSReport("Running 1.03 loader\n");

	patch_crash_screen();

	// Zero out the bussy
	memset(&__BSS_START__, 0, (size_t)&__BSS_SIZE__);

	InitCardBuffers();

#if defined(PAL) || defined(NTSC102)
	// Use GALE01 saves for PAL/UP
	__CARDSetDiskID("GALE01");
#endif

#ifndef NTSC102
	const auto *data = alloc_and_read("103Code");
	const auto *base = get_file(data, 0);
#if defined(NTSC100)
	const auto *diff = get_file(data, 1);
#elif defined(NTSC101)
	const auto *diff = get_file(data, 2);
#elif defined(PAL)
	const auto *diff = get_file(data, 3);
#endif
	const auto code_size = apply_diff(base->data, diff->data, &__MOD_BASE__);
#else
	auto *buffer = &__MOD_BASE__;
	card_read("103Code", buffer);

	const auto *base = get_file(buffer, 0);
	const auto code_size = base->uncompressed_len;

	// Copy compressed data past end of space reserved for uncompressed data and decompress
	auto *compressed = (char*)base->data + base->uncompressed_len;
	memcpy(compressed, base->data, base->uncompressed_len);
	decompress(compressed, base->compressed_len, buffer, code_size);
#endif

#if defined(PAL) || defined(NTSC102)
	// Restore gamecode
	__CARDSetDiskID(__GameCode);
#endif

	DCStoreRange(&__MOD_BASE__, code_size);
	ICInvalidateRange(&__MOD_BASE__, code_size);

	// Use a different save file name
	// Overwriting the end retains 20XX's modified save name
	strcpy(SaveFileName + strlen(SaveFileName) - 4, "_103");

	// Clear nametags
	memset(NameTag_GetEntry(0), 0, sizeof(NameTagEntry) * NAMETAG_MAX);

	// Run mod init
	OSReport("Running 1.03\n");
	((void(*)())&__MOD_BASE__)();

	// Load new save file
	MemoryCard_DoLoadData();

	// Apply sound balance from new save file
	ApplySoundBalance();

	if (SceneMajor == Scene_VsMode)
		Scene_SetMajorPending(Scene_VsMode); // Reload CSS
	else
		*(u8*)Scene_GetExitData() = Scene_VsMode; // Boot to CSS

	Scene_Exit();
}
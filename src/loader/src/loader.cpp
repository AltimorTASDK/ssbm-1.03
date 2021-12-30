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

extern "C" {

extern void *CardWorkArea;

extern char SaveFileName[25];

extern char __LOAD_BASE__;
extern char __MOD_BASE__;
extern char __COPY_START__;
extern char __COPY_SIZE__;

extern u8 CSSPendingSceneChange;

void OSEnableScheduler();

s32 OSSaveContext(OSContext *context);
void OSDefaultErrorHandler(OSContext *context);
s32 DisplayCrashScreen(OSContext *context);

void InitCardBuffers();

u32 MemoryCard_DoLoadData();

}

static card_file file;
static card_stat stats;
static wait_object wait;
static void *read_dest;
static const char *read_file;

extern "C" char __BSS_START__;
extern "C" char __BSS_SIZE__;

static void panic(const char *fmt, auto &&...args)
{
	OSReport(fmt, args...);

	// Enable interrupts so VI interrupts happen
	OSRestoreInterrupts(1);

	// Enable scheduler so crash screen thread runs
	OSEnableScheduler();

	OSContext context;
	OSSaveContext(&context);
	OSDefaultErrorHandler(&context);
}

static void card_callback(s32 chan, s32 result)
{
	if (result < 0) {
		panic("Card read failed (%d)\n", result);
		return;
	}

	// Wake up the main thread
	wait.wake();
}

static void read_callback(s32 chan, s32 result)
{
	if (result < 0) {
		panic("Card mount failed (%d)\n", result);
		return;
	}

	if (s32 error = CARD_Open(chan, read_file, &file); error < 0) {
		panic("CARD_Open failed (%d)\n", error);
		return;
	}

	if (s32 error = CARD_GetStatus(chan, file.filenum, &stats); error < 0) {
		panic("CARD_GetStatus failed (%d)\n", error);
		return;
	}

	if (read_dest == nullptr) {
		// Wake up the main thread
		wait.wake();
		return;
	}

	if (s32 error = CARD_ReadAsync(&file, read_dest, stats.len, 0, card_callback); error < 0) {
		panic("CARD_ReadAsync failed (%d)\n", error);
		return;
	}
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

	if (s32 error = CARD_MountAsync(0, CardWorkArea, nullptr, read_callback); error < 0) {
		panic("CARD_MountAsync failed (%d)\n", error);
		return 0;
	}

	// Wait for read to complete
	wait.sleep();

	CARD_Unmount(0);

	return stats.len;
}

static void *alloc_and_read(const char *file)
{
	const auto size = card_read(file, nullptr);
	auto *buf = HSD_MemAlloc(size);
	card_read(file, buf);
	return buf;
}

struct file_entry {
	u32 size;
	char data[];
};

const file_entry *get_file(void *data, int index)
{
	for (auto i = 0; i < index; i++)
		data = (char*)data + ((file_entry*)data)->size + 4;

	return (file_entry*)data;
}

extern "C" [[gnu::section(".loader")]] void load_mod()
{
	OSReport("Running 1.03 loader\n");

	patch_crash_screen();

	// Zero out the bussy
	memset(&__BSS_START__, 0, (size_t)&__BSS_SIZE__);

	InitCardBuffers();

#ifdef PAL
	// Use GALE01 saves
	cardmap[0].gamecode[3] = 'E';
#endif

	auto *data = alloc_and_read("103Code");

	auto *base = get_file(data, 0);

#ifndef NTSC102
#if defined(NTSC100)
	auto *diff = get_file(data, 1);
#elif defined(NTSC101)
	auto *diff = get_file(data, 2);
#elif defined(PAL)
	auto *diff = get_file(data, 3);
#endif
	const auto code_size = apply_diff(base->data, diff->data, &__MOD_BASE__);
#else
	const auto code_size = base->size;
	memcpy(&__MOD_BASE__, base->data, code_size);
#endif

#ifdef PAL
	// Restore PAL gamecode
	cardmap[0].gamecode[3] = 'P';
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

	if (SceneMajor == Scene_VsMode)
		Scene_SetMajorPending(Scene_VsMode); // Reload CSS
	else
		*(u8*)Scene_GetExitData() = Scene_VsMode; // Boot to CSS

	Scene_Exit();
}
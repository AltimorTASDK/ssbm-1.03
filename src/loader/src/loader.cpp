#include "hsd/memory.h"
#include "os/os.h"
#include "os/thread.h"
#include "util/gc/wait_object.h"
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <gctypes.h>
#include <ogc/card.h>

extern "C" {

void TRK_flush_cache(const void *start, u32 size);

void OSEnableScheduler();

s32 OSSaveContext(OSContext *context);
void OSDefaultErrorHandler(OSContext *context);
s32 DisplayCrashScreen(OSContext *context);

void InitCardBuffers();

extern void *CardWorkArea;

}

static void *mod_init = (void*)0x817B1000;

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

	TRK_flush_cache(patch_location, 4);
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

#ifndef NTSC102
static void *alloc_and_read(const char *file)
{
	const auto size = card_read(file, nullptr);
	auto *buf = HSD_MemAlloc(size);
	card_read(file, buf);
	return buf;
}

static u32 apply_diff(const char *base, const char *diff, char *out)
{
	enum {
		CMD_EOF              = 0,
		CMD_DATA_LEN16       = 247,
		CMD_DATA_LEN32       = 248,
		CMD_COPY_POS16_LEN8  = 249,
		CMD_COPY_POS16_LEN16 = 250,
		CMD_COPY_POS16_LEN32 = 251,
		CMD_COPY_POS32_LEN8  = 252,
		CMD_COPY_POS32_LEN16 = 253,
		CMD_COPY_POS32_LEN32 = 254
	};

	char *start = out;

	while (*diff != CMD_EOF) {
		switch (*diff) {
		default: {
			const auto len = *(u8*)diff;
			memcpy(out, diff + 1, len);
			out += len;
			diff += 1 + len;
			break;
		}
		case CMD_DATA_LEN16: {
			const auto len = *(u16*)(diff + 1);
			memcpy(out, diff + 3, len);
			out += len;
			diff += 3 + len;
			break;
		}
		case CMD_DATA_LEN32: {
			const auto len = *(u32*)(diff + 1);
			memcpy(out, diff + 5, len);
			out += len;
			diff += 5 + len;
			break;
		}
		case CMD_COPY_POS16_LEN8: {
			const auto pos = *(u16*)(diff + 1);
			const auto len = *(u8*)(diff + 3);
			memcpy(out, base + pos, len);
			out += len;
			diff += 4;
			break;
		}
		case CMD_COPY_POS16_LEN16: {
			const auto pos = *(u16*)(diff + 1);
			const auto len = *(u16*)(diff + 3);
			memcpy(out, base + pos, len);
			out += len;
			diff += 5;
			break;
		}
		case CMD_COPY_POS16_LEN32: {
			const auto pos = *(u16*)(diff + 1);
			const auto len = *(u32*)(diff + 3);
			memcpy(out, base + pos, len);
			out += len;
			diff += 7;
			break;
		}
		case CMD_COPY_POS32_LEN8: {
			const auto pos = *(u32*)(diff + 1);
			const auto len = *(u8*)(diff + 5);
			memcpy(out, base + pos, len);
			out += len;
			diff += 6;
			break;
		}
		case CMD_COPY_POS32_LEN16: {
			const auto pos = *(u32*)(diff + 1);
			const auto len = *(u16*)(diff + 5);
			memcpy(out, base + pos, len);
			out += len;
			diff += 7;
			break;
		}
		case CMD_COPY_POS32_LEN32: {
			const auto pos = *(u32*)(diff + 1);
			const auto len = *(u32*)(diff + 5);
			memcpy(out, base + pos, len);
			out += len;
			diff += 9;
			break;
		}
		}
	}

	return (u32)(out - start);
}
#endif

extern "C" [[gnu::section(".init")]] void _start()
{
	OSReport("Running 1.03 loader\n");

	patch_crash_screen();

	// Zero out the bussy
	memset(&__BSS_START__, 0, (size_t)&__BSS_SIZE__);

	InitCardBuffers();

#ifdef NTSC102
	const auto code_size = card_read("103Code", mod_init);
#else
	auto *base = alloc_and_read("103Code");
#ifdef NTSC100
	auto *diff = alloc_and_read("103CodeNTSC100");
#endif
	const auto code_size = apply_diff((char*)base, (char*)diff, (char*)mod_init);
#endif

	TRK_flush_cache(mod_init, code_size);

	// Run mod init
	OSReport("Running 1.03\n");
	((void(*)())mod_init)();
}
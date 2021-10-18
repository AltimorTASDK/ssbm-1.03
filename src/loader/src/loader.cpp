#include <cstddef>
#include <cstdio>
#include <cstring>
#include <ogc/card.h>

struct OSContext {
	char pad000[0x2C8];
};

struct OSThread {
	OSContext ctx;
	char pad2C8[0x310 - 0x2C8];
};

struct OSThreadQueue {
	OSThread *next;
	OSThread *prev;
};

extern "C" {

void OSReport(const char *fmt, ...);

void OSRestoreInterrupts(u32 level);

void OSEnableScheduler();
void OSSleepThread(OSThreadQueue *queue);
void OSWakeupThread(OSThreadQueue *queue);

s32 OSSaveContext(OSContext *context);
void OSDefaultErrorHandler(OSContext *context);
s32 DisplayCrashScreen(OSContext *context);

void InitCardBuffers();

extern void *CardWorkArea;

}

static void *mod_init = (void*)0x817E1000;

static card_file file;
static OSThreadQueue sleep_queue;

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

static void read_callback(s32 chan, s32 result)
{
	if (result < 0) {
		panic("Card read failed (%d)\n", result);
		return;
	}
	
	CARD_Unmount(chan);

	// Run mod init
	OSReport("Running 1.03\n");
	((void(*)())mod_init)();
	
	// Wake up the main thread
	OSWakeupThread(&sleep_queue);
}

static void attach_callback(s32 chan, s32 result)
{
	if (s32 error = CARD_Open(chan, "103Code", &file); error < 0) {
		panic("CARD_Open failed (%d)\n", error);
		return;
	}

	card_stat stats;
	if (s32 error = CARD_GetStatus(chan, file.filenum, &stats); error < 0) {
		panic("CARD_GetStatus failed (%d)\n", error);
		return;
	}

	if (s32 error = CARD_ReadAsync(&file, mod_init, stats.len, 0, read_callback); error < 0) {
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

	__builtin___clear_cache(patch_location, patch_location + 4);
}

extern "C" __attribute__((section(".init"))) void _start()
{
	OSReport("Running 1.03 loader\n");
	
	patch_crash_screen();
	
	// Zero out the bussy
	memset(&__BSS_START__, 0, (size_t)&__BSS_SIZE__);

	InitCardBuffers();
	
	if (s32 error = CARD_MountAsync(0, CardWorkArea, nullptr, attach_callback); error < 0) {
		panic("CARD_MountAsync failed (%d)\n", error);
		return;
	}
	
	OSSleepThread(&sleep_queue);
}
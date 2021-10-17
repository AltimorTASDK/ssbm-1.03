#include <cstddef>
#include <cstring>
#include <ogc/card.h>

extern "C" void *HSD_MemAlloc(size_t size);
extern "C" void HSD_Free(void *ptr);
extern "C" void OSReport(const char *fmt, ...);
extern "C" void InitCardBuffers();

extern "C" void *CardWorkArea;

static bool *loader_done;
static void *mod_init = (void*)0x817E1000;
static card_file file;

extern "C" char __BSS_START__;
extern "C" char __BSS_SIZE__;

static void read_callback(s32 chan, s32 result)
{
	if (result < 0) {
		OSReport("Card read failed (%d)\n", result);
		return;
	}
	
	CARD_Unmount(0);

	// Run mod init
	OSReport("Running 1.03\n");
	((void(*)())mod_init)();
	
	// Tell ace_exploit.mgc to continue
	*loader_done = true;
}

static void attach_callback(s32 chan, s32 result)
{
	if (s32 error = CARD_Open(chan, "103Code", &file); error < 0) {
		OSReport("CARD_Open failed (%d)\n", error);
		return;
	}

	card_stat stats;
	if (s32 error = CARD_GetStatus(chan, file.filenum, &stats); error < 0) {
		OSReport("CARD_GetStatus failed (%d)\n", error);
		return;
	}

	if (s32 error = CARD_ReadAsync(&file, mod_init, stats.len, 0, read_callback); error < 0)
		OSReport("CARD_ReadAsync failed (%d)\n", error);
}

extern "C" __attribute__((section(".init"))) void _start(bool *done)
{
	OSReport("Running 1.03 loader\n");
	
	// Zero out the bussy
	memset(&__BSS_START__, 0, (size_t)&__BSS_SIZE__);

	loader_done = done;
	*done = false;

	InitCardBuffers();
	
	if (s32 error = CARD_MountAsync(0, CardWorkArea, nullptr, attach_callback); error < 0)
		OSReport("CARD_MountAsync failed (%d)\n", error);
}
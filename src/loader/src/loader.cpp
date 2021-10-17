#include <cstddef>
#include <ogc/card.h>

extern "C" void *HSD_MemAlloc(size_t size);
extern "C" void HSD_Free(void *ptr);
extern "C" void OSReport(const char *fmt, ...);

void *mod_init = (void*)0x817E1000;

static void *workarea;

static void read_callback(s32 chan, s32 result)
{
	// Unmount/free
	CARD_Unmount(0);
	HSD_Free(workarea);
	
	// Run mod init
	OSReport("Running 1.03\n");
	((void(*)())mod_init)();
}

static void attach_callback(s32 chan, s32 result)
{
	card_file file;
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
		OSReport("CARD_Read failed (%d)\n", error);
}

__attribute__((section(".init"))) void init()
{
	workarea = HSD_MemAlloc(0xA000);
	
	if (s32 error = CARD_MountAsync(0, workarea, nullptr, attach_callback); error < 0)
		OSReport("CARD_MountAsync failed (%d)\n", error);
}
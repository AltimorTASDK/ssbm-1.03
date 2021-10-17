#include "hsd/memory.h"
#include "os/os.h"
#include <cstring>
#include <ogc/card.h>

extern "C" int SetupMemCard();

static bool done = false;
static void *workarea;
static char *read_buf;

static void read_callback(s32 chan, s32 result)
{
	OSReport("read\n");
	
	OSReport("card read buf\n");
	for (auto i = 0; i < 16; i++)
		OSReport("%02X ", read_buf[i]);

	OSReport("\n");
	
	done = true;
}

static void attach_callback(s32 chan, s32 result)
{
	OSReport("attach result %d\n", result);
		
	card_file file;
	if (s32 error = CARD_Open(chan, "SuperSmashBros0110290334", &file); error < 0) {
		OSReport("CARD_Open failed (%d)\n", error);
		return;
	}

	OSReport("open\n");
	
	card_stat stats;
	if (s32 error = CARD_GetStatus(chan, file.filenum, &stats); error < 0) {
		OSReport("CARD_GetStatus failed (%d)\n", error);
		return;
	}

	read_buf = (char*)HSD_MemAlloc(stats.len);
	memset(read_buf, 0xAA, stats.len);

	OSReport("len %08X\n", stats.len);
	OSReport("buf %p\n", read_buf);

	if (s32 error = CARD_ReadAsync(&file, read_buf, stats.len, 0, read_callback); error < 0) {
		OSReport("CARD_Read failed (%d)\n", error);
		return;
	}
}

struct loader_test {
	loader_test()
	{
		OSReport("hi\n");
		
		workarea = HSD_MemAlloc(0xA000);
		
		OSReport("hi %p\n", workarea);
		
		if (s32 error = CARD_MountAsync(0, workarea, nullptr, attach_callback); error < 0) {
			OSReport("CARD_MountAsync failed (%d)\n", error);
			return;
		}
	}
} loader_test;
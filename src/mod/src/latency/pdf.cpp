#include "hsd/pad.h"
#include "hsd/video.h"
#include "os/alarm.h"
#include "os/thread.h"
#include "os/vi.h"
#include "util/patch_list.h"

extern "C" OSAlarm PadFetchAlarm;

extern "C" void PadFetchCallback();

static const auto patches = patch_list {
	// Abandon EFB copy if no XFB is ready to avoid getting stuck with 1f delay
	// b +0xBC
	std::pair { (char*)HSD_VICopyXFBASync+0x34, 0x480000BCu },
};

extern "C" void orig_UpdatePadFetchRate();
extern "C" void hook_UpdatePadFetchRate()
{
	// Don't use the fetch timer
	orig_UpdatePadFetchRate();
	OSCancelAlarm(&PadFetchAlarm);
}

#if 0
static void retrace_callback(u32 retrace_count)
{
	PadFetchCallback();
}

struct set_retrace_cb {
	set_retrace_cb()
	{
		HSD_VISetUserPostRetraceCallback(retrace_callback);
	}
} set_retrace_cb;
#endif

/*extern "C" void orig_VIRetraceHandler(u32 irq, OSContext *context);
extern "C" void hook_VIRetraceHandler(u32 irq, OSContext *context)
{
	if (vi_regs->display_int[2].bits.status)
		PadFetchCallback();
	
	orig_VIRetraceHandler(irq, context);
}

struct set_display_interrupt {
	set_display_interrupt()
	{
		constexpr auto interrupt = display_interrupt {
			.bits = {
				.enable = true,
				.position_v = 50,
				.position_h = 429
			}
		};
		
		vi_regs->display_int[2].word = interrupt.word;
	}
} set_display_interrupt;*/
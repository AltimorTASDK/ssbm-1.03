#include "hsd/pad.h"
#include "hsd/video.h"
#include "os/alarm.h"
#include "os/si.h"
#include "os/thread.h"
#include "os/vi.h"
#include "util/patch_list.h"

extern "C" OSAlarm PadFetchAlarm;

extern "C" void PadFetchCallback();

/*static const auto patches = patch_list {
	// Abandon EFB copy if no XFB is ready to avoid getting stuck with 1f delay
	// b +0xBC
	std::pair { (char*)HSD_VICopyXFBASync+0x34, 0x480000BCu },
};*/

extern "C" void orig_UpdatePadFetchRate();
extern "C" void hook_UpdatePadFetchRate()
{
	// Don't use the fetch timer
	orig_UpdatePadFetchRate();
	OSCancelAlarm(&PadFetchAlarm);
}

static void retrace_callback(u32 retrace_count)
{
	// Poll on retrace in LOW latency mode if there won't be SI reads
	if (Si.poll.enable == 0)
		PadFetchCallback();
}

struct set_retrace_cb {
	set_retrace_cb()
	{
		HSD_VISetUserPostRetraceCallback(retrace_callback);
	}
} set_retrace_cb;
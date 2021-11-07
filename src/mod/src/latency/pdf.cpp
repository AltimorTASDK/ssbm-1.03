#include "hsd/pad.h"
#include "hsd/video.h"
#include "os/alarm.h"
#include "os/si.h"
#include "os/thread.h"
#include "os/vi.h"
#include "rules/values.h"
#include "util/patch_list.h"

extern "C" OSAlarm PadFetchAlarm;

extern "C" void PadFetchCallback();

extern "C" void orig_UpdatePadFetchRate();
extern "C" void hook_UpdatePadFetchRate()
{
	// Don't use the fetch timer
	orig_UpdatePadFetchRate();
	OSCancelAlarm(&PadFetchAlarm);
}

static void retrace_callback(u32 retrace_count)
{
	// Poll on retrace in normal latency mode or if there won't be SI reads
	if (get_latency() == latency_mode::normal || Si.poll.enable == 0)
		PadFetchCallback();
}

struct set_retrace_cb {
	set_retrace_cb()
	{
		HSD_VISetUserPostRetraceCallback(retrace_callback);
	}
} set_retrace_cb;
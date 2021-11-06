#include "hsd/pad.h"
#include "hsd/video.h"
#include "os/alarm.h"

extern "C" OSAlarm PadFetchAlarm;

extern "C" void PadFetchCallback();

extern "C" void orig_UpdatePadFetchRate();
extern "C" void hook_UpdatePadFetchRate()
{
	// Don't use the fetch timer
	orig_UpdatePadFetchRate();
	OSCancelAlarm(&PadFetchAlarm);
}

static void pre_retrace_callback(u32 retrace_count)
{
	PadFetchCallback();
}

struct set_retrace_cb {
	set_retrace_cb()
	{
		HSD_VISetUserPreRetraceCallback(pre_retrace_callback);
	}
} set_retrace_cb;
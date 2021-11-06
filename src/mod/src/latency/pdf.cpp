#include "os/os.h"
#include "os/vi.h"

static u64 fetch_time;
static u64 poll_time;

extern "C" void *PADSetSamplingCallback(void(*callback)());

static double to_msecs(u64 interval)
{
	return (double)interval / 40500.0;
}

static void pad_sample_callback()
{
	const auto last_time = poll_time;
	poll_time = OSGetTime();
	OSReport("poll interval %.04f\n", to_msecs(poll_time - last_time));
	OSReport("poll line %d\n", VIGetCurrentLine());
}

struct set_pad_cb {
	set_pad_cb()
	{
		PADSetSamplingCallback(pad_sample_callback);
	}
} set_pad_cb;

extern "C" void orig_PadFetchCallback();
extern "C" void hook_PadFetchCallback()
{
	const auto last_time = fetch_time;
	fetch_time = OSGetTime();
	OSReport("fetch interval %.04f\n", to_msecs(fetch_time - last_time));
	OSReport("si read -> fetch %.04f\n", to_msecs(fetch_time - poll_time));
	OSReport("fetch line %d\n", VIGetCurrentLine());
	orig_PadFetchCallback();
}

extern "C" void orig_HSD_PerfSetStartTime();
extern "C" void hook_HSD_PerfSetStartTime()
{
	//OSReport("poll age %llu\n", OSGetTime() - fetch_time);
	orig_HSD_PerfSetStartTime();
}

extern "C" void scene_loop_start()
{
	while (VIGetCurrentLine() < 240)
		;
}
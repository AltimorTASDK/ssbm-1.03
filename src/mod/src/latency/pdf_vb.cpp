#include "hsd/cobj.h"
#include "hsd/gobj.h"
#include "hsd/pad.h"
#include "hsd/video.h"
#include "latency/pdf_vb.h"
#include "melee/text.h"
#include "os/alarm.h"
#include "os/os.h"
#include "os/serial.h"
#include "os/thread.h"
#include "os/vi.h"
#include "rules/values.h"
#include <gctypes.h>

extern "C" OSAlarm PadFetchAlarm;

extern "C" void *PADSetSamplingCallback(void(*callback)());
extern "C" void PadFetchCallback();

#ifdef POLL_DEBUG
constexpr auto TEXT_WIDTH = 24;
#ifdef POLL_DEBUG_VERBOSE
constexpr auto TEXT_HEIGHT = 13;
#else
constexpr auto TEXT_HEIGHT = 3;
#endif

static u32 poll_line[2];
static u64 poll_interval[2];

static u64 poll_time;
static u64 poll_time_queue[PAD_QNUM];

static u64 fetch_time;
static u32 fetch_line;
static u64 fetch_time_queue[PAD_QNUM];
static u64 fetch_interval;

static u64 frame_time;
static u32 frame_line;
static u64 frame_poll_time;
static u64 frame_fetch_time;

static u64 frame_interval;
static u32 frame_end_line;

static u32 efb_copy_late;
static u32 efb_copy_line;
static u64 efb_copy_poll_time[3];

static u64 retrace_time;
static u64 retrace_poll_time;

static DevText *text;
static char text_buf[TEXT_WIDTH * TEXT_HEIGHT * 2];
#endif

static bool faster_melee;

static bool needs_depth_clear;

static u32 frame_retrace_count;
static u32 last_poll_retrace_count;

static u32 half_vb_retrace_count;
static OSThreadQueue half_vb_thread_queue;

extern "C" bool is_faster_melee()
{
	return faster_melee;
}

static void pad_sample_callback()
{
#ifdef POLL_DEBUG
	const auto current_poll_time = OSGetTime();
	const auto current_poll_line = VIGetCurrentLine();
#endif
	const auto current_retrace_count = VIGetRetraceCount();
	const auto index = current_retrace_count > last_poll_retrace_count ? 0 : 1;

#ifdef POLL_DEBUG
	poll_line[index] = current_poll_line;
	poll_interval[index] = current_poll_time - poll_time;
	poll_time = current_poll_time;
#endif

	last_poll_retrace_count = current_retrace_count;

	if (index == 0 && get_latency() != latency_mode::crt) {
		// Use first poll rather than previous mid-frame poll for LCD/LOW
		PadFetchCallback();
	} else if (index == 1 && get_latency() == latency_mode::lcd) {
		// Delay processing (and audio+rumble) by half a frame on LCD
		half_vb_retrace_count = VIGetRetraceCount();
		OSWakeupThread(&half_vb_thread_queue);
	}
}

static void post_retrace_callback(u32 retrace_count)
{
	if (get_latency() == latency_mode::crt) {
		// Fetch on retrace in CRT mode
		PadFetchCallback();
		return;
	}

	// Check poll retrace count as well to catch Dolphin/Nintendont hacks
	if (Si.poll.enable != 0 || last_poll_retrace_count >= retrace_count - 1)
		return;

	// Fetch on retrace in LCD/LOW if there are no SI reads
	PadFetchCallback();

	// Wake game thread when polling is disabled so it doesn't hang
	if (get_latency() == latency_mode::lcd)
		OSWakeupThread(&half_vb_thread_queue);
}

extern "C" void orig_UpdatePadFetchRate();
extern "C" void hook_UpdatePadFetchRate()
{
	if (is_faster_melee())
		return;

	// Don't use the fetch timer
	orig_UpdatePadFetchRate();
	OSCancelAlarm(&PadFetchAlarm);
}

extern "C" void orig_SISetSamplingRate(u32 msecs);
extern "C" void hook_SISetSamplingRate(u32 msecs)
{
	if (is_faster_melee())
		return;

	// Always use default polling rate/interval
	orig_SISetSamplingRate(16);
}

extern "C" void scene_loop_start()
{
	if (is_faster_melee())
		return;

	if (get_latency() != latency_mode::lcd || Si.poll.enable == 0)
		return;

	const auto irq_enable = OSDisableInterrupts();

	// Wait for 2nd poll to start processing in LCD mode unless we're already late
	if (VIGetRetraceCount() != half_vb_retrace_count)
		OSSleepThread(&half_vb_thread_queue);

	OSRestoreInterrupts(irq_enable);
}

extern "C" void first_engine_frame()
{
#ifdef POLL_DEBUG
	frame_time = OSGetTime();
	frame_line = VIGetCurrentLine();
#endif
	frame_retrace_count = VIGetRetraceCount();
}

extern "C" void orig_HSD_VICopyXFBASync(u32 pass);
extern "C" void hook_HSD_VICopyXFBASync(u32 pass)
{
	if (is_faster_melee())
		return orig_HSD_VICopyXFBASync(pass);

#ifdef POLL_DEBUG
	frame_interval = OSGetTime() - frame_time;
	frame_end_line = VIGetCurrentLine();
#endif

	const auto irq_enable = OSDisableInterrupts();

	// Artifically induce VB if not on LOW latency
	if (VIGetRetraceCount() == frame_retrace_count && get_latency() != latency_mode::low)
		VIWaitForRetrace();

	// Skip EFB copy if no XFB is ready and process a new frame instead
	if (HSD_VIGetXFBDrawEnable() == -1) {
		// Force depth clear because we're skipping GX_CopyDisp
		needs_depth_clear = true;
		VIWaitForRetrace();
		OSRestoreInterrupts(irq_enable);
		return;
	}

	needs_depth_clear = false;

	OSRestoreInterrupts(irq_enable);
	orig_HSD_VICopyXFBASync(pass);

#ifdef POLL_DEBUG
	if (get_latency() != latency_mode::low) {
		if (VIGetRetraceCount() > frame_retrace_count + 1)
			efb_copy_late = 10;
	} else {
		if (VIGetRetraceCount() > frame_retrace_count)
			efb_copy_late = 10;
	}
#endif
}

extern "C" void orig_HSD_CObjEraseScreen(HSD_CObj *cobj, bool color, bool alpha, bool depth);
extern "C" void hook_HSD_CObjEraseScreen(HSD_CObj *cobj, bool color, bool alpha, bool depth)
{
	if (needs_depth_clear)
		depth = true;

	orig_HSD_CObjEraseScreen(cobj, color, alpha, depth);
}

#ifdef POLL_DEBUG
extern "C" void orig_HSD_PerfSetStartTime();
extern "C" void hook_HSD_PerfSetStartTime()
{
	frame_poll_time = poll_time_queue[HSD_PadLibData.qread];
	frame_fetch_time = fetch_time_queue[HSD_PadLibData.qread];
	orig_HSD_PerfSetStartTime();
}

extern "C" void orig_PadFetchCallback();
extern "C" void hook_PadFetchCallback()
{
	const auto last_time = fetch_time;
	fetch_time = OSGetTime();
	fetch_line = VIGetCurrentLine();
	poll_time_queue[HSD_PadLibData.qwrite] = poll_time;
	fetch_time_queue[HSD_PadLibData.qwrite] = fetch_time;
	fetch_interval = fetch_time - last_time;
	orig_PadFetchCallback();
}

extern "C" s32 orig_HSD_VIGetXFBDrawEnable();
extern "C" s32 hook_HSD_VIGetXFBDrawEnable()
{
	const auto xfb = orig_HSD_VIGetXFBDrawEnable();

	if (xfb != -1) {
		efb_copy_poll_time[xfb] = frame_poll_time;
		efb_copy_line = VIGetCurrentLine();
	}

	return xfb;
}

extern "C" void orig_HSD_VIPreRetraceCB(u32 retrace_count);
extern "C" void hook_HSD_VIPreRetraceCB(u32 retrace_count)
{
	orig_HSD_VIPreRetraceCB(retrace_count);

	const auto xfb = HSD_VISearchXFBByStatus(HSD_VI_XFB_NEXT);

	if (xfb != -1) {
		retrace_time = OSGetTime();
		retrace_poll_time = efb_copy_poll_time[xfb];
	}
}

static double ms(u64 interval)
{
	const auto bus_speed = *(volatile u32*)0x800000F8;
	return (double)interval / ((double)bus_speed / 4000.0);
}

static void update_text(HSD_GObj *gobj)
{
	if (efb_copy_late != 0) {
		DevelopText_SetColorIndex(text, 1);
		efb_copy_late--;
	} else {
		DevelopText_SetColorIndex(text, 0);
	}

	DevelopText_Erase(text);
	DevelopText_SetCursor(text, 0, 0);
#ifdef POLL_DEBUG_VERBOSE
	DevelopText_Printf(text, "Poll -> Fetch   %.04f\n", ms(frame_fetch_time - frame_poll_time));
#endif
	DevelopText_Printf(text, "Poll -> Engine  %.04f\n", ms(frame_time - frame_poll_time));
	DevelopText_Printf(text, "Poll -> Display %.04f\n", ms(retrace_time - retrace_poll_time));
#ifdef POLL_DEBUG_VERBOSE
	DevelopText_Printf(text, "Fetch Interval  %.04f\n", ms(fetch_interval));
	DevelopText_Printf(text, "Engine Time     %.04f\n", ms(frame_interval));
	DevelopText_Printf(text, "Poll 1 Interval %.04f\n", ms(poll_interval[0]));
	DevelopText_Printf(text, "Poll 2 Interval %.04f\n", ms(poll_interval[1]));
	DevelopText_Printf(text, "Fetch Line      %d\n",    fetch_line);
	DevelopText_Printf(text, "Engine Line     %d\n",    frame_line);
	DevelopText_Printf(text, "Engine End Line %d\n",    frame_end_line);
	DevelopText_Printf(text, "EFB Copy Line   %d\n",    efb_copy_line);
	DevelopText_Printf(text, "Poll 1 Line     %d\n",    poll_line[0]);
	DevelopText_Printf(text, "Poll 2 Line     %d",      poll_line[1]);
#else
	DevelopText_Printf(text, "Engine Time     %.04f",   ms(frame_interval));
#endif
}

extern "C" void orig_Scene_RunLoop(void(*think_callback)());
extern "C" void hook_Scene_RunLoop(void(*think_callback)())
{
	text = DevelopText_Create(0x69, 0, 0, TEXT_WIDTH, TEXT_HEIGHT, text_buf);
	DevelopText_Show(nullptr, text);
	DevelopText_HideCursor(text);
	DevelopText_SetBGColor(text, color_rgba::hex(0x00000060u));
	DevelopText_SetColorIndex(text, 1);
	DevelopText_SetTextColor(text, color_rgba::hex(0xFF0000FFu));
	DevelopText_SetColorIndex(text, 0);
	DevelopText_SetTextColor(text, color_rgba::hex(0xFFFFFFFFu));
#ifdef POLL_DEBUG_VERBOSE
	DevelopText_SetScale(text, 9, 12);
#else
	DevelopText_SetScale(text, 12, 16);
#endif

	auto *gobj = GObj_Create(GOBJ_CLASS_UI, GOBJ_PLINK_UI, 0);
	GObj_AddProc(gobj, update_text, 0);

	orig_Scene_RunLoop(think_callback);
}
#endif

static bool detect_fm_impl()
{
	constexpr auto magic = 0xFFFFFFFF;

	SICHANNEL[0].in.lo = magic;

	if (SICHANNEL[0].in.lo != magic) {
		// Console ignores writes to SIC0INBUF
		OSReport("Detected console\n");
		return false;
	}

	SICHANNEL[0].in.hi = magic;

	if (SICHANNEL[0].in.hi == magic) {
		// Mainline Dolphin allows SIC0INBUFH writes to persist
		OSReport("Detected mainline Dolphin\n");
		return false;
	} else {
		// FM renews inputs when SIC0INBUFH is read
		OSReport("Detected FM Dolphin\n");
		return true;
	}
}

static bool detect_fm()
{
	const auto irq_enable = OSDisableInterrupts();
	const auto sipoll = Si.poll.raw;
	SIDisablePolling(0b11110000 << 24);
	const auto result = detect_fm_impl();
	SIEnablePolling(sipoll << 24);
	OSRestoreInterrupts(irq_enable);

	return result;
}

struct set_callbacks {
	set_callbacks()
	{
		if (detect_fm()) {
			faster_melee = true;
			return;
		}

		PADSetSamplingCallback(pad_sample_callback);
		HSD_VISetUserPostRetraceCallback(post_retrace_callback);
	}
} set_callbacks;
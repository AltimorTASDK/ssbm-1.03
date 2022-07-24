#include "hsd/cobj.h"
#include "hsd/gobj.h"
#include "hsd/pad.h"
#include "hsd/video.h"
#include "latency/polling.h"
#include "melee/text.h"
#include "os/alarm.h"
#include "os/os.h"
#include "os/serial.h"
#include "os/thread.h"
#include "os/vi.h"
#include "rules/values.h"
#include <gctypes.h>

extern "C" OSAlarm PadFetchAlarm;

extern "C" void PadFetchCallback();

constexpr auto TEXT_WIDTH = 24;
#ifdef POLL_DEBUG
#ifdef POLL_DEBUG_VERBOSE
constexpr auto TEXT_HEIGHT = 13;
#else // POLL_DEBUG_VERBOSE
constexpr auto TEXT_HEIGHT = 3;
#endif // POLL_DEBUG_VERBOSE
#else // POLL_DEBUG
constexpr auto TEXT_HEIGHT = 1;
#endif // POLL_DEBUG

static bool faster_melee;
static bool checked_fm;

static bool needs_depth_clear;

static bool is_polling;
static u32 no_poll_frames;

static bool simulated_this_frame;
static s32 poll_index;

static OSThreadQueue half_vb_thread_queue;

static DevText *text;
static char text_buf[TEXT_WIDTH * TEXT_HEIGHT * 2];

#ifdef POLL_DEBUG
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
#endif // POLL_DEBUG

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
	SI_DisablePolling(0b11110000 << 24);
	const auto result = detect_fm_impl();
	SI_EnablePolling(sipoll << 24);
	OSRestoreInterrupts(irq_enable);

	return result;
}

extern "C" bool is_faster_melee()
{
	if (!checked_fm) {
		faster_melee = detect_fm();
		checked_fm = true;
	}

	return faster_melee;
}

void post_retrace_callback(u32 retrace_count)
{
	// Check if there were any poll interrupts this frame
	if (poll_index == 0)
		is_polling = false;
	else
		poll_index = 0;

	if (Si.poll.enable != 0 && !is_polling)
		no_poll_frames++;
	else
		no_poll_frames = 0;

	simulated_this_frame = false;

	// Fetch on retrace in CRT or in LCD/LOW if there are no poll interrupts
	if (get_latency() != latency_mode::crt && is_polling)
		return;

	PadFetchCallback();

	// Wake game thread when polling is disabled so it doesn't hang
	if (get_latency() == latency_mode::lcd)
		OSWakeupThread(&half_vb_thread_queue);
}

extern "C" bool orig_SI_GetResponseRaw(s32 chan);
extern "C" bool hook_SI_GetResponseRaw(s32 chan)
{
	const auto result = orig_SI_GetResponseRaw(chan);

	if (is_faster_melee())
		return result;

	// Wait until all ports are polled
	if (chan != 3)
		return result;

#ifdef POLL_DEBUG
	if (poll_index < 2) {
		const auto current_poll_time = OSGetTime();
		const auto current_poll_line = VIGetCurrentLine();
		poll_line[poll_index] = current_poll_line;
		poll_interval[poll_index] = current_poll_time - poll_time;
		poll_time = current_poll_time;
	}
#endif

	if (poll_index == 0 && get_latency() != latency_mode::crt) {
		// Use first poll rather than previous mid-frame poll for LCD/LOW
		PadFetchCallback();
	} else if (poll_index == Si.poll.y / 2 && get_latency() == latency_mode::lcd) {
		// Delay processing (and audio+rumble) by half a frame on LCD
		OSWakeupThread(&half_vb_thread_queue);
	}

	is_polling = true;
	poll_index++;

	return result;
}

extern "C" void orig_UpdatePadFetchRate();
extern "C" void hook_UpdatePadFetchRate()
{
	orig_UpdatePadFetchRate();

	// Don't use the fetch timer
	if (!is_faster_melee())
		OSCancelAlarm(&PadFetchAlarm);
}

extern "C" void orig_SI_SetSamplingRate(u32 msecs);
extern "C" void hook_SI_SetSamplingRate(u32 msecs)
{
	// Always use default polling rate/interval
	if (!is_faster_melee())
		msecs = 16;

	orig_SI_SetSamplingRate(msecs);
}

extern "C" void scene_loop_start()
{
	if (is_faster_melee())
		return;

	if (get_latency() != latency_mode::lcd || !is_polling)
		return;

	const auto irq_enable = OSDisableInterrupts();

	// Wait for 2nd poll to start processing in LCD mode unless we're already late
	if (poll_index <= Si.poll.y / 2)
		OSSleepThread(&half_vb_thread_queue);

	OSRestoreInterrupts(irq_enable);
}

extern "C" void first_engine_frame()
{
#ifdef POLL_DEBUG
	frame_time = OSGetTime();
	frame_line = VIGetCurrentLine();
#endif
	simulated_this_frame = true;
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
	if (simulated_this_frame && get_latency() != latency_mode::low)
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
#endif

static void update_text(HSD_GObj *gobj)
{
#ifdef POLL_DEBUG
	DevelopText_Erase(text);
	DevelopText_SetCursor(text, 0, 0);
	DevelopText_SetColorIndex(text, 0);

	if (efb_copy_late != 0) {
		DevelopText_SetColorIndex(text, 1);
		efb_copy_late--;
	}

#ifdef POLL_DEBUG_VERBOSE
	DevelopText_Printf(text, "Poll -> Fetch   %.04f\n", ms(frame_fetch_time - frame_poll_time));
#endif // POLL_DEBUG_VERBOSE
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
#else // POLL_DEBUG_VERBOSE
	DevelopText_Printf(text, "Engine Time     %.04f",   ms(frame_interval));
#endif // POLL_DEBUG_VERBOSE
#else // POLL_DEBUG
	if (get_latency() != latency_mode::crt && no_poll_frames >= 2) {
		DevelopText_ShowText(text);
		DevelopText_ShowBackground( text);
	} else {
		DevelopText_HideText(text);
		DevelopText_HideBackground(text);
	}
#endif
}

extern "C" void orig_Scene_RunLoop(void(*think_callback)());
extern "C" void hook_Scene_RunLoop(void(*think_callback)())
{
	text = DevelopText_Create(0x69, 20, 20, TEXT_WIDTH, TEXT_HEIGHT, text_buf);
	if (text == nullptr)
		return orig_Scene_RunLoop(think_callback);

	DevelopText_HideCursor(text);
	DevelopText_SetBGColor(text, color_rgba::hex(0x00000060u));
	DevelopText_Show(nullptr, text);

#ifdef POLL_DEBUG
	DevelopText_SetColorIndex(text, 1);
	DevelopText_SetTextColor(text, color_rgba::hex(0xFF0000FFu));
	DevelopText_SetColorIndex(text, 0);
	DevelopText_SetTextColor(text, color_rgba::hex(0xFFFFFFFFu));
#ifdef POLL_DEBUG_VERBOSE
	DevelopText_SetScale(text, 9, 12);
#else // POLL_DEBUG_VERBOSE
	DevelopText_SetScale(text, 12, 16);
#endif // POLL_DEBUG_VERBOSE
#else // POLL_DEBUG
	DevelopText_SetTextColor(text, color_rgba::hex(0xFF0000FFu));
	DevelopText_SetScale(text, 18, 24);
	DevelopText_HideText(text);
	DevelopText_HideBackground(text);
	DevelopText_Print(text, "Latency reduction error");
#endif // POLL_DEBUG

	auto *gobj = GObj_Create(GOBJ_CLASS_UI, GOBJ_PLINK_DEV_TEXT, 0);
	GObj_AddProc(gobj, update_text, 0);

	orig_Scene_RunLoop(think_callback);
}

[[gnu::constructor]] static void init_latency()
{
	if (is_faster_melee())
		return;

	SI_EnablePollingInterrupt(true);
	HSD_VISetUserPostRetraceCallback(post_retrace_callback);
}
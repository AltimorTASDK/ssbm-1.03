#include "hsd/gobj.h"
#include "hsd/pad.h"
#include "hsd/video.h"
#include "melee/text.h"
#include "os/os.h"
#include "os/vi.h"
#include <gctypes.h>

constexpr auto TEXT_WIDTH = 24;
constexpr auto TEXT_HEIGHT = 9;

static u32 last_poll_line;
static u32 poll_line[2];
static u64 poll_interval[2];

static u64 poll_time;
static u64 poll_time_queue[PAD_QNUM];

static u64 fetch_time;
static u64 fetch_time_queue[PAD_QNUM];
static u64 fetch_interval;

static u64 frame_time;
static u64 frame_line;
static u64 frame_poll_time;
static u64 frame_fetch_time;

static u64 draw_poll_time[3];

static u64 retrace_time;
static u64 retrace_poll_time;

static DevText *text;
static char text_buf[TEXT_WIDTH * TEXT_HEIGHT * 2];

extern "C" void *PADSetSamplingCallback(void(*callback)());

static void pad_sample_callback()
{
	const auto current_poll_time = OSGetTime();
	const auto current_poll_line = VIGetCurrentLine();
	const auto index = current_poll_line < last_poll_line ? 0 : 1;

	poll_line[index] = current_poll_line;
	poll_interval[index] = current_poll_time - poll_time;

	last_poll_line = current_poll_line;
	poll_time = current_poll_time;
}

extern "C" void orig_PadFetchCallback();
extern "C" void hook_PadFetchCallback()
{
	const auto last_time = fetch_time;
	fetch_time = OSGetTime();
	poll_time_queue[HSD_PadLibData.qwrite] = poll_time;
	fetch_time_queue[HSD_PadLibData.qwrite] = fetch_time;
	fetch_interval = fetch_time - last_time;
	orig_PadFetchCallback();
}

extern "C" void orig_HSD_PerfSetStartTime();
extern "C" void hook_HSD_PerfSetStartTime()
{
	frame_time = OSGetTime();
	frame_line = VIGetCurrentLine();
	frame_poll_time = poll_time_queue[HSD_PadLibData.qread];
	frame_fetch_time = fetch_time_queue[HSD_PadLibData.qread];
	orig_HSD_PerfSetStartTime();
}

extern "C" s32 orig_HSD_VIWaitXFBDrawEnable();
extern "C" s32 hook_HSD_VIWaitXFBDrawEnable()
{
	const auto index = orig_HSD_VIWaitXFBDrawEnable();
	if (index != -1)
		draw_poll_time[index] = frame_poll_time;
		
	return index;
}

extern "C" void orig_HSD_VIPreRetraceCB(u32 retrace_count);
extern "C" void hook_HSD_VIPreRetraceCB(u32 retrace_count)
{
	orig_HSD_VIPreRetraceCB(retrace_count);
	
	const auto xfb = HSD_VISearchXFBByStatus(HSD_VI_XFB_NEXT);
	if (xfb == -1)
		return;
	
	retrace_time = OSGetTime();
	retrace_poll_time = draw_poll_time[xfb];
}

static double ms(u64 interval)
{
	return (double)interval / 40500.0;
}

static void update_text(HSD_GObj *gobj)
{
	DevelopText_Erase(text);
	DevelopText_SetCursor(text, 0, 0);
	DevelopText_Printf(text, "Poll -> Fetch   %.04f\n", ms(frame_fetch_time - frame_poll_time));
	DevelopText_Printf(text, "Poll -> Engine  %.04f\n", ms(frame_time - frame_poll_time));
	DevelopText_Printf(text, "Poll -> Display %.04f\n", ms(retrace_time - retrace_poll_time));
	DevelopText_Printf(text, "Fetch Interval  %.04f\n", ms(fetch_interval));
	DevelopText_Printf(text, "Engine Line     %d\n",    frame_line);
	DevelopText_Printf(text, "Poll 1 Interval %.04f\n", ms(poll_interval[0]));
	DevelopText_Printf(text, "Poll 2 Interval %.04f\n", ms(poll_interval[1]));
	DevelopText_Printf(text, "Poll 1 Line     %d\n",    poll_line[0]);
	DevelopText_Printf(text, "Poll 2 Line     %d",      poll_line[1]);
}

extern "C" HSD_GObj *orig_DevelopText_Setup(u32 classfier, u32 p_link, u32 p_prio, u32 gx_link,
                                            u32 render_priority, u8 camera_priority);
extern "C" HSD_GObj *hook_DevelopText_Setup(u32 classfier, u32 p_link, u32 p_prio, u32 gx_link,
                                            u32 render_priority, u8 camera_priority)
{
	auto *gobj = orig_DevelopText_Setup(classfier, p_link, p_prio, gx_link,
	                                    render_priority, camera_priority);
	
	text = DevelopText_Create(0x69, 0, 0, TEXT_WIDTH, TEXT_HEIGHT, text_buf);
	DevelopText_Show(nullptr, text);
	DevelopText_HideCursor(text);
	DevelopText_SetBGColor(text, { 0, 0, 0, 96 });
	DevelopText_SetTextColor(text, color_rgba::white);
	DevelopText_SetScale(text, 9, 12);
	
	GObj_AddProc(gobj, update_text, 0);
	
	return gobj;
}

struct set_pad_cb {
	set_pad_cb()
	{
		PADSetSamplingCallback(pad_sample_callback);
	}
} set_pad_cb;
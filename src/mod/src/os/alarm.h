#pragma once

#include <gctypes.h>

using OSAlarmCallback = void();

struct OSAlarm {
	OSAlarmCallback *handler;
	u32 tag;
	u64 fire;
	OSAlarm *prev;
	OSAlarm *next;
	u64 period;
	u64 start;
};

extern "C" {
	
void OSCancelAlarm(OSAlarm *alarm);
	
}
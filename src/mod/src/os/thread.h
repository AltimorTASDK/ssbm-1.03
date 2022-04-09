#pragma once

struct OSContext {
	char pad000[0x2C8];
};

struct OSThread {
	OSContext ctx;
	char pad2C8[0x310 - 0x2C8];
};

struct OSThreadQueue {
	OSThread *next;
	OSThread *prev;
};

extern "C" {

extern OSThread *CurrentThread;

void OSSleepThread(OSThreadQueue *queue);
void OSWakeupThread(OSThreadQueue *queue);

} // extern "C"
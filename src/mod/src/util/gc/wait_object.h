#pragma once

#include "os/os.h"
#include "os/thread.h"

class wait_object {
	bool complete = true;
	OSThreadQueue queue;
	
public:
	void reset()
	{
		complete = false;
	}
	
	void sleep()
	{
		const auto irq_enable = OSDisableInterrupts();

		if (CurrentThread != nullptr) {
			// Don't sleep if operation already completed
			if (!complete)
				OSSleepThread(&queue);

			OSRestoreInterrupts(irq_enable);
		} else {
			// Fall back to busywait
			OSRestoreInterrupts(irq_enable);

			while (!complete)
				continue;
		}
	}
	
	void wake()
	{
		complete = true;
		OSWakeupThread(&queue);
	}
	
	bool is_complete() const
	{
		return complete;
	}
};
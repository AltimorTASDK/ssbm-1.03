#pragma once

#include "os/os.h"
#include "os/thread.h"

class wait_object {
	bool waiting;
	OSThreadQueue queue;

public:
	void reset()
	{
		waiting = true;
	}

	void sleep()
	{
		const auto irq_enable = OSDisableInterrupts();

		if (CurrentThread != nullptr) {
			// Don't sleep if operation already completed
			if (waiting)
				OSSleepThread(&queue);

			OSRestoreInterrupts(irq_enable);
		} else {
			// Fall back to busywait
			OSRestoreInterrupts(irq_enable);

			while (waiting)
				continue;
		}
	}

	void wake()
	{
		waiting = false;
		OSWakeupThread(&queue);
	}

	bool is_complete() const
	{
		return !waiting;
	}
};
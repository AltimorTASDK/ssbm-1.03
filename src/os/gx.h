#pragma once

#include "os/os.h"
#include "util/types.h"
#include <ogc/gx.h>

inline union {
private:
	volatile u8 as_u8;
	volatile u32 as_u32;
	volatile f32 as_f32;

public:
	void write(u8 value) { as_u8 = value; }
	void write(u32 value) { as_u32 = value; }
	void write(f32 value) { as_f32 = value; }
	void write(auto ...values) { (write(values), ...); }
} *gx_fifo = (decltype(gx_fifo))0xCC008000;
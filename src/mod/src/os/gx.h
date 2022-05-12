#pragma once

#include <gctypes.h>
#include <ogc/gx.h>

inline union {
private:
	volatile u8  as_u8;
	volatile u16 as_u16;
	volatile u32 as_u32;
	volatile f32 as_f32;

	void write_impl(u8  value) { as_u8  = value; }
	void write_impl(u16 value) { as_u16 = value; }
	void write_impl(u32 value) { as_u32 = value; }
	void write_impl(f32 value) { as_f32 = value; }

public:
	void write(auto ...values) { (write_impl(values), ...); }
} *gx_fifo = (decltype(gx_fifo))0xCC008000;
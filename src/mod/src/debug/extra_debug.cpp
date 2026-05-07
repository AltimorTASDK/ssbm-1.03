#include "dolphin/context.h"
#include "dolphin/os.h"
#include <cmath>

extern "C" void orig_PrintStackTrace(const OSContext *context, u32 max_depth);
extern "C" void hook_PrintStackTrace(const OSContext *context, u32 max_depth)
{
	orig_PrintStackTrace(context, max_depth);

	const auto *stack = (u32*)context->gpr[1];

	OSReport("1.03 Stack Dump ------------------\n");
	for (auto i = -16; i <= 16; i++) {
		const auto gpr = i + 16;
		if (gpr < 32)
			OSReport("stack%s0x%02X: 0x%08X             GPR%02d: 0x%08X\n", i >= 0 ? "+" : "-", std::abs(i * 4), stack[i], gpr, context->gpr[gpr]);
		else
			OSReport("stack%s0x%02X: 0x%08X\n", i >= 0 ? "+" : "-", std::abs(i * 4), stack[i]);
	}
	OSReport("----------------------------------\n");
}
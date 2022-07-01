#pragma once

#include <gctypes.h>

enum FtCmd {
	FtCmd_SetFlag = 19
};

struct FtCmdArg_SetFlag {
	u32 opcode : 6;
	u32 flag   : 2;
	u32 value  : 24;
};

struct FtCmdState {
	f32 timer;
	f32 frame;
	u8 *script;
	u32 stack_depth;
	u8 *stack[5];
};

extern "C" {

bool FtCmd_ControlFlow(FtCmdState *ftcmd, u32 opcode);

extern u8 FtCmdLength_Player[49];

} // extern "C"
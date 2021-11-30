#pragma once

enum DbLKind {
	DbLKind_Master       = 0,
	DbLKind_NoDebugRom   = 1,
	DbLKind_DebugDevelop = 2,
	DbLKind_DebugRom     = 3,
	DbLKind_Develop      = 4
};

enum DebugEntry {
	DebugEntry_Language = 3,
	DebugEntry_Publicity = 4,
	DebugEntry_DbLevel = 5,
	DebugEntry_IKDebugFlag = 6,
	DebugEntry_NewDefCalc = 7
};

struct DebugMenuEntry {
	u32 type;
	void(*on_change)();
	char *label;
	char **options;
	u32 *value;
	char pad014[0x20 - 0x14];
};

extern "C" DebugMenuEntry DebugMenuEntries[10];
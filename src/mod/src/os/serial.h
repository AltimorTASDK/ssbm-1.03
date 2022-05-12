#pragma once

#include <gctypes.h>
#include <ogc/si.h>
#include "util/vector.h"

enum SIKey
{
	KEY_HOME = 0x06,
	KEY_END = 0x07,
	KEY_PGUP = 0x08,
	KEY_PGDN = 0x09,
	KEY_SCROLLLOCK = 0x0A,
	KEY_A = 0x10,
	KEY_B = 0x11,
	KEY_C = 0x12,
	KEY_D = 0x13,
	KEY_E = 0x14,
	KEY_F = 0x15,
	KEY_G = 0x16,
	KEY_H = 0x17,
	KEY_I = 0x18,
	KEY_J = 0x19,
	KEY_K = 0x1A,
	KEY_L = 0x1B,
	KEY_M = 0x1C,
	KEY_N = 0x1D,
	KEY_O = 0x1E,
	KEY_P = 0x1F,
	KEY_Q = 0x20,
	KEY_R = 0x21,
	KEY_S = 0x22,
	KEY_T = 0x23,
	KEY_U = 0x24,
	KEY_V = 0x25,
	KEY_W = 0x26,
	KEY_X = 0x27,
	KEY_Y = 0x28,
	KEY_Z = 0x29,
	KEY_1 = 0x2A,
	KEY_2 = 0x2B,
	KEY_3 = 0x2C,
	KEY_4 = 0x2D,
	KEY_5 = 0x2E,
	KEY_6 = 0x2F,
	KEY_7 = 0x30,
	KEY_8 = 0x31,
	KEY_9 = 0x32,
	KEY_0 = 0x33,
	KEY_MINUS = 0x34,
	KEY_PLUS = 0x35,
	KEY_PRINTSCR = 0x36,
	KEY_BRACE_OPEN = 0x37,
	KEY_BRACE_CLOSE = 0x38,
	KEY_COLON = 0x39,
	KEY_QUOTE = 0x3A,
	KEY_HASH = 0x3B,
	KEY_COMMA = 0x3C,
	KEY_PERIOD = 0x3D,
	KEY_QUESTIONMARK = 0x3E,
	KEY_INTERNATIONAL1 = 0x3F,
	KEY_F1 = 0x40,
	KEY_F2 = 0x41,
	KEY_F3 = 0x42,
	KEY_F4 = 0x43,
	KEY_F5 = 0x44,
	KEY_F6 = 0x45,
	KEY_F7 = 0x46,
	KEY_F8 = 0x47,
	KEY_F9 = 0x48,
	KEY_F10 = 0x49,
	KEY_F11 = 0x4A,
	KEY_F12 = 0x4B,
	KEY_ESC = 0x4C,
	KEY_INSERT = 0x4D,
	KEY_DELETE = 0x4E,
	KEY_TILDE = 0x4F,
	KEY_BACKSPACE = 0x50,
	KEY_TAB = 0x51,
	KEY_CAPSLOCK = 0x53,
	KEY_LEFTSHIFT = 0x54,
	KEY_RIGHTSHIFT = 0x55,
	KEY_LEFTCONTROL = 0x56,
	KEY_RIGHTALT = 0x57,
	KEY_LEFTWINDOWS = 0x58,
	KEY_SPACE = 0x59,
	KEY_RIGHTWINDOWS = 0x5A,
	KEY_MENU = 0x5B,
	KEY_LEFTARROW = 0x5C,
	KEY_DOWNARROW = 0x5D,
	KEY_UPARROW = 0x5E,
	KEY_RIGHTARROW = 0x5F,
	KEY_ENTER = 0x61
};

union SIPoll {
	u32 raw;
	struct {
		u32 : 6;
		u32 x : 10;
		u32 y : 8;
		u32 enable : 4;
		u32 vblank_copy : 4;
	};
};

struct SIControl {
	char pad000[0x04];
	SIPoll poll;
	char pad008[0x14 - 0x08];
};

struct SIStatus {
	u8 wr : 1;
	u8 : 1;
	u8 rdst : 1;
	u8 wrst : 1;
	u8 norep : 1;
	u8 coll : 1;
	u8 ovrun : 1;
	u8 unrun : 1;
};

struct SIChannel {
	u32 out;
	union {
		struct {
			u32 hi;
			u32 lo;
		};
		struct {
			u16 errstat : 1;
			u16 errlatch : 1;
			u16 buttons : 14;
			s8 stick_x;
			s8 stick_y;
			s8 cstick_x;
			s8 cstick_y;
			u8 analog_l;
			u8 analog_r;
		};
	} in;
};

struct SIKeyboard {
	u32 counter;
	u8 keys[3];
	u8 checksum;
};

inline volatile auto *SICHANNEL = (SIChannel*)0xCC006400;
inline volatile auto *SISR = (SIStatus*)0xCC006438;

extern "C" SIControl Si;
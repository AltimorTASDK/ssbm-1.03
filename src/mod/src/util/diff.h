#pragma once

#include <cstring>
#include <gctypes.h>

inline u32 apply_diff(const char *base, const char *diff, char *out)
{
	enum {
		CMD_EOF              = 0,
		CMD_DATA_LEN16       = 247,
		CMD_DATA_LEN32       = 248,
		CMD_COPY_POS16_LEN8  = 249,
		CMD_COPY_POS16_LEN16 = 250,
		CMD_COPY_POS16_LEN32 = 251,
		CMD_COPY_POS32_LEN8  = 252,
		CMD_COPY_POS32_LEN16 = 253,
		CMD_COPY_POS32_LEN32 = 254
	};

	char *start = out;
	u32 offset = 0;

	while (*diff != CMD_EOF) {
		switch (*diff) {
		default: {
			const auto len = *(u8*)diff;
			if (out != nullptr)
				memcpy(out + offset, diff + 1, len);
			offset += len;
			diff += 1 + len;
			break;
		}
		case CMD_DATA_LEN16: {
			const auto len = *(u16*)(diff + 1);
			if (out != nullptr)
				memcpy(out + offset, diff + 3, len);
			offset += len;
			diff += 3 + len;
			break;
		}
		case CMD_DATA_LEN32: {
			const auto len = *(u32*)(diff + 1);
			if (out != nullptr)
				memcpy(out + offset, diff + 5, len);
			offset += len;
			diff += 5 + len;
			break;
		}
		case CMD_COPY_POS16_LEN8: {
			const auto pos = *(u16*)(diff + 1);
			const auto len = *(u8*)(diff + 3);
			if (out != nullptr)
				memcpy(out + offset, base + pos, len);
			offset += len;
			diff += 4;
			break;
		}
		case CMD_COPY_POS16_LEN16: {
			const auto pos = *(u16*)(diff + 1);
			const auto len = *(u16*)(diff + 3);
			if (out != nullptr)
				memcpy(out + offset, base + pos, len);
			offset += len;
			diff += 5;
			break;
		}
		case CMD_COPY_POS16_LEN32: {
			const auto pos = *(u16*)(diff + 1);
			const auto len = *(u32*)(diff + 3);
			if (out != nullptr)
				memcpy(out + offset, base + pos, len);
			offset += len;
			diff += 7;
			break;
		}
		case CMD_COPY_POS32_LEN8: {
			const auto pos = *(u32*)(diff + 1);
			const auto len = *(u8*)(diff + 5);
			if (out != nullptr)
				memcpy(out + offset, base + pos, len);
			offset += len;
			diff += 6;
			break;
		}
		case CMD_COPY_POS32_LEN16: {
			const auto pos = *(u32*)(diff + 1);
			const auto len = *(u16*)(diff + 5);
			if (out != nullptr)
				memcpy(out + offset, base + pos, len);
			offset += len;
			diff += 7;
			break;
		}
		case CMD_COPY_POS32_LEN32: {
			const auto pos = *(u32*)(diff + 1);
			const auto len = *(u32*)(diff + 5);
			if (out != nullptr)
				memcpy(out + offset, base + pos, len);
			offset += len;
			diff += 9;
			break;
		}
		}
	}

	return offset;
}
#pragma once

#include "util/math.h"
#include <gctypes.h>
#include <ogc/card.h>
#include <utility>

constexpr auto CARD_WRITE_ALIGN = 0x2000;
constexpr auto CARD_READ_ALIGN  = 0x200;

// Alignment padded memcard buffer
template<typename T, size_t align>
class [[gnu::aligned(32)]] card_buffer {
	T value;
	char pad[align_up(sizeof(T), align) - sizeof(T)] = { 0 };

public:
	card_buffer(auto &&...args) : value(std::forward<decltype(args)>(args)...)
	{
	}

	T &operator *() { return  value; }
	T *operator->() { return &value; }
};

template<typename T>
using card_write_buffer = card_buffer<T, CARD_WRITE_ALIGN>;
template<typename T>
using card_read_buffer  = card_buffer<T, CARD_READ_ALIGN>;

// Sets result to positive file size or negative error. out may be null.
void card_read(s32 card, const char *filename, void *out, u32 max_size);

// Sets result to negative error.
void card_write(s32 card, const char *filename, void *in, u32 size);

// Check if a card operation is in progress.
bool is_card_busy();

// Wait for card operations to complete. Returns operation result.
s32 card_sync();
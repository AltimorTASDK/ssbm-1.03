#pragma once

#include "util/math.h"
#include <gctypes.h>
#include <ogc/card.h>

// Sets result to positive file size or negative error. out may be null.
void card_read(s32 card, const char *filename, void *out, u32 max_size);

// Sets result to negative error.
void card_write(s32 card, const char *filename, void *in, u32 size);

// Cancel any current read/write operation.
void card_cancel();

// Wait for card operations to complete. Returns operation result.
s32 card_sync();
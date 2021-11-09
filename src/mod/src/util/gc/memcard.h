#pragma once

#include "util/math.h"
#include <gctypes.h>
#include <ogc/card.h>

// Returns positive file size or negative error. out may be null.
s32 card_read(s32 card, const char *filename, void *out, u32 max_size);

// Returns negative error.
s32 card_write(s32 card, const char *filename, void *in, u32 size);

// Wait for card operations to complete.
void card_sync();
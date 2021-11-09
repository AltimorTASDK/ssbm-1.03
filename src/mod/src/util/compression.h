#include "hsd/tobj.h"
#include "util/mempool.h"
#include "util/draw/texture.h"
#include <gctypes.h>

// Automatically restore old texture params on free
class texture_swap {
	HSD_ImageDesc image_copy;
	const u8 *buffer;
public:
	HSD_ImageDesc *image;

	texture_swap(const u8 *data, HSD_ImageDesc *image = nullptr);
	~texture_swap();
};

u8 *decompress(const u8 *data, void *out = nullptr);

// Swap texture for the rest of a scene. Returns buffer if a new one was allocated.
u8 *unmanaged_texture_swap(const u8 *data, HSD_ImageDesc *image);

// Returns buffer
u8 *init_texture(const u8 *data, texture *out);
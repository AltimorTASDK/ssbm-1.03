#include "hsd/tobj.h"
#include "os/os.h"
#include "util/compression.h"
#include <cstddef>
#include <cstring>
#include <gctypes.h>
#include <ogc/gx.h>

struct index_header {
	u16 uncompressed_len;
	u8 bits;
	u8 table_size;
	u8 table[];
};

struct rle_data {
	u16 uncompressed_len;
	u8 data[];
};

struct compressed_file {
	u8 is_indexed;
	union {
		rle_data rle;
		index_header indexed;
	};
};

struct resource_header {
	u16 header_size;
};

struct tex_header : resource_header {
	u16 width;
	u16 height;
	u8 format;
};

// Allocates a buffer
u8 *rle_decode(const rle_data *rle, u8 *out = nullptr)
{
	const auto *in = rle->data;
	const auto out_len = rle->uncompressed_len;
	
	if (out == nullptr)
		out = new u8[out_len];
	
	size_t in_idx = 0;
	size_t out_idx = 0;

	while (out_idx < out_len) {
		// Run:       0x00 | length, byte
		// Raw bytes: 0x80 | length, bytes...
		constexpr auto raw_mask = 0x80;

		if (in[in_idx] & raw_mask) {
			// Raw bytes
			const auto count = in[in_idx] & ~raw_mask;
			const auto *data = &in[in_idx + 1];
			memcpy(&out[out_idx], data, count);

			out_idx += count;
			in_idx += count + 1;
		} else {
			// Run
			const auto count = in[in_idx];
			const auto value = in[in_idx + 1];
			memset(&out[out_idx], value, count);

			out_idx += count;
			in_idx += 2;
		}
	}
	
	DCFlushRange(out, out_len);
	
	return out;
}

// Allocates a buffer
u8 *index_decode(const index_header *index, u8 *out = nullptr)
{
	const auto out_len = index->uncompressed_len;
	const auto bits = index->bits;
	const auto mask = (1 << bits) - 1;
	const auto *table = index->table;

	const auto *rle = (rle_data*)(table + index->table_size);
	const auto *in = rle_decode(rle);

	if (out == nullptr)
		out = new u8[out_len];

	size_t bitpos = 0;
	
	for (size_t out_idx = 0; out_idx < out_len; out_idx++) {
		const auto in_idx = bitpos / 8;
		const auto offset = bitpos % 8;
		
		if (offset + bits <= 8) {
			const auto index = (in[in_idx] >> (8 - offset - bits)) & mask;
			out[out_idx] = table[index];
		} else {
			// Take 16 bits at a time to deal with crossing byte boundaries
			const auto word = (in[in_idx] << 8) | in[in_idx + 1];
			const auto index = (word >> (16 - offset - bits)) & mask;
			out[out_idx] = table[index];
		}
		
		bitpos += bits;
	}
	
	delete[] in;
	
	DCFlushRange(out, out_len);
	
	return out;
}

u8 *decompress(const u8 *data, void *out)
{
	const auto *resource = (resource_header*)data;
	const auto *body = data + resource->header_size;
	const auto *compressed = (compressed_file*)body;
	if (compressed->is_indexed)
		return index_decode(&compressed->indexed, (u8*)out);
	else
		return rle_decode(&compressed->rle, (u8*)out);
}

texture_swap::texture_swap(const u8 *data, HSD_ImageDesc *image)
	: image(image)
{
	if (image != nullptr) {
		image_copy = *image;
		buffer = unmanaged_texture_swap(data, image);
	} else {
		image_copy = { 0 };
		this->image = &image_copy;
		buffer = unmanaged_texture_swap(data, &image_copy);
	}
}

texture_swap::~texture_swap()
{
	if (buffer != nullptr)
		delete[] buffer;
		
	if (image != &image_copy)
		*image = image_copy;
}

u8 *unmanaged_texture_swap(const u8 *data, HSD_ImageDesc *image)
{
	const auto *tex = (tex_header*)data;
	
	const auto old_size = GX_GetTexBufferSize(image->width, image->height, image->format,
	                                          (u8)image->mipmap, (u8)image->max_lod);

	const auto new_size = GX_GetTexBufferSize(tex->width, tex->height, tex->format,
	                                          GX_FALSE, 0);
						  
	image->width = tex->width;
	image->height = tex->height;
	image->format = tex->format;
						  
	if (new_size > old_size || image->img_ptr == nullptr) {
		// Allocate a new buffer
		image->img_ptr = decompress(data);
		return (u8*)image->img_ptr;
	} else {
		// Copy over existing texture
		decompress(data, image->img_ptr);
		return nullptr;
	}
}

u8 *init_texture(const u8 *data, texture *out)
{
	const auto *tex = (tex_header*)data;
	auto *buffer = decompress(data);
	out->init(buffer, tex->width, tex->height, tex->format);
	return buffer;
}
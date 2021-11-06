#include "os/os.h"
#include "util/compression.h"
#include <cstddef>
#include <cstring>
#include <gctypes.h>

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
	u16 is_indexed;
	union {
		rle_data rle;
		index_header indexed;
	};
};

// Allocates a buffer
u8 *rle_decode(const rle_data *data)
{
	const auto *in = data->data;
	const auto out_len = data->uncompressed_len;
	auto *out = new u8[out_len];
	
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
u8 *index_decode(const index_header *index)
{
	const auto out_len = index->uncompressed_len;
	const auto bits = index->bits;
	const auto mask = (1 << bits) - 1;
	const auto *table = index->table;

	const auto *rle = (rle_data*)(table + index->table_size);
	const auto *in = rle_decode(rle);
	
	auto *out = new u8[out_len];

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

u8 *decompress(const u8 *data)
{
	const auto *header = (compressed_file*)data;
	if (header->is_indexed)
		return index_decode(&header->indexed);
	else
		return rle_decode(&header->rle);
}
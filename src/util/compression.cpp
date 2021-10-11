#include "util/compression.h"
#include <cstddef>
#include <cstring>

// Returns decoded length
size_t rle_decode(const unsigned char *in, size_t in_len, unsigned char *out)
{
	size_t in_idx = 0;
	size_t out_idx = 0;

	while (in_idx < in_len) {
		// Run:       0x00 | length, byte
		// Raw bytes: 0x80 | length, bytes...
		constexpr auto raw_mask = 0x80;

		if (in[in_idx] & raw_mask) {
			// Raw bytes
			const auto count = in[in_idx] & ~raw_mask;
			const auto *data = &in[in_idx + 1];

			if (out != nullptr)
				memcpy(&out[out_idx], data, count);

			out_idx += count;
			in_idx += count + 1;
		} else {
			// Run
			const auto count = in[in_idx];
			const auto value = in[in_idx + 1];

			if (out != nullptr)
				memset(&out[out_idx], value, count);

			out_idx += count;
			in_idx += 2;
		}
	}
	
	return out_idx;
}

unsigned char *rle_decode(const unsigned char *data, size_t length)
{
	const auto out_len = rle_decode(data, length, nullptr);

	auto *out = new unsigned char[out_len];
	rle_decode(data, length, out, nullptr);

	return out;
}
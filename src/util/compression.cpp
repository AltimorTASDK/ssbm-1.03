#include "util/compression.h"
#include <cstddef>
#include <cstring>

void rle_decode(const unsigned char *in, size_t in_len, unsigned char *out, size_t *out_len)
{
	size_t in_idx = 0;
	size_t out_idx = 0;
	while (in_idx < in_len) {
		if (in[in_idx] & 0x80) {
			const auto count = in[in_idx] & ~0x80;

			if (out != nullptr)
				memcpy(&out[out_idx], &in[in_idx + 1], count);

			out_idx += count;
			in_idx += count + 1;
		} else {
			const auto count = in[in_idx];

			if (out != nullptr)
				memset(&out[out_idx], in[in_idx + 1], count);

			out_idx += count;
			in_idx += 2;
		}
	}
	
	if (out_len != nullptr)
		*out_len = out_idx;
}

unsigned char *rle_decode(const unsigned char *data, size_t length)
{
	size_t out_len;
	rle_decode(data, length, nullptr, &out_len);

	auto *out = new unsigned char[out_len];
	rle_decode(data, length, out, nullptr);
	return out;
}
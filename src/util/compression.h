#include <cstddef>

size_t rle_decode(const unsigned char *in, size_t in_len, unsigned char *out);
unsigned char *rle_decode(const unsigned char *data, size_t length);

template<size_t N>
inline unsigned char *rle_decode(const unsigned char (&data)[N])
{
	return rle_decode(data, N);
}
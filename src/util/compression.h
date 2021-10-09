#include <cstddef>

void rle_decode(const unsigned char *in, size_t in_len, unsigned char *out, size_t *out_len);
unsigned char *rle_decode(const unsigned char *data, size_t length);

template<size_t N>
inline unsigned char *rle_decode(const unsigned char (&data)[N])
{
	return rle_decode(data, N);
}
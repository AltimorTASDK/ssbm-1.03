import os
import struct
import sys
from itertools import compress, takewhile

def rle_encode(data):
    i = 0
    # Prepend uncompressed length
    out = bytearray(struct.pack(">I", len(data)))

    while i < len(data):
        run = len([*takewhile(
            lambda j: data[i] == data[j],
            range(i + 1, min(len(data), i + 0x7F)))]) + 1

        if run > 1:
            out += bytes([run, data[i]])
        else:
            run = len([*takewhile(
                lambda j: j == len(data) - 1 or data[j] != data[j + 1],
                range(i + 1, min(len(data), i + 0x7F)))]) + 1

            out += bytes([run | 0x80])
            out += data[i:i+run]

        i += run

    return out

def index_encode(data, bits, table):
    bitpos = 0
    out = bytearray()

    for b in data:
        index = table.index(b)
        # Start from most significant bit
        shifted = index << (8 - bits)
        offset = bitpos % 8

        if offset == 0:
            out += bytes([shifted])
        else:
            out[-1] |= shifted >> offset
            if offset > 8 - bits:
                # Shift to next byte
                out += bytes([((shifted << 8) >> offset) & 0xFF])

        bitpos += bits

    return out

def compress_rle(data):
    # 0 = not indexed
    return bytearray(struct.pack(">Bxxx", 0)) + rle_encode(data)

def compress_indexed(data):
    unique_bytes = [b for b in range(0x100) if b in data]
    index_size = len(unique_bytes)
    index_bits = (index_size - 1).bit_length()

    if index_bits == 8:
        # No point indexing
        return None

    # 1 = indexed
    indexed = bytearray(struct.pack(">Bxxx", 1))

    # Uncompressed size
    indexed += struct.pack(">I", len(data))

    # Index table
    # 2 byte align
    index_table = unique_bytes
    index_table += b"\x00" * ((2 - len(index_table) % 2) % 2)
    indexed += bytes([index_bits])
    indexed += bytes([len(index_table)])
    indexed += bytes(index_table)

    # Indexed data
    indexed += rle_encode(index_encode(data, index_bits, unique_bytes))
    return indexed

def compress(data):
    # Choose best compression strategy
    rle = compress_rle(data)
    indexed = compress_indexed(data)

    if indexed is None or len(rle) < len(indexed):
        return rle
    else:
        return indexed

def get_header_size(extension):
    return {
        '.tex': 6
    }.get(extension, 0)

def main():
    if len(sys.argv) < 3:
        print("Usage: compress_resource.py <in> <out>", file=sys.stderr)
        sys.exit(1)

    in_path = sys.argv[1]
    out_path = sys.argv[2]

    with open(in_path, "rb") as f:
        data = f.read()

    header_size = get_header_size(os.path.splitext(in_path)[1])
    header = data[:header_size]
    body = data[header_size:]

    compressed = compress(body)

    name = os.path.basename(out_path)
    total_size = 2 + len(compressed) + len(header)
    compression_rate = (1 - total_size / len(data)) * 100

    print(f"{name} compression rate: "
          f"{compression_rate:.02f}% "
          f"({total_size} bytes)")

    with open(out_path, "wb") as f:
        # Include the size of the header_size field itself
        f.write(struct.pack(">H", header_size + 2))
        f.write(header)
        f.write(compressed)

if __name__ == "__main__":
    main()
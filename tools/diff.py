import struct
import sys

# https://www.w3.org/TR/NOTE-gdiff-19970901

CMD_EOF              = 0
CMD_DATA_LEN16       = 247
CMD_DATA_LEN32       = 248
CMD_COPY_POS16_LEN8  = 249
CMD_COPY_POS16_LEN16 = 250
CMD_COPY_POS16_LEN32 = 251
CMD_COPY_POS32_LEN8  = 252
CMD_COPY_POS32_LEN16 = 253
CMD_COPY_POS32_LEN32 = 254

def make_diff(a, b, out):
    offset = 0

    def cmd_data(pos, length):
        if length == 0:
            return
        data = b[pos:pos+length]
        if length >= 2**16:
            out.write(struct.pack(">BI", CMD_DATA_LEN32, length))
        elif length >= CMD_DATA_LEN16:
            out.write(struct.pack(">BH", CMD_DATA_LEN16, length))
        else:
            out.write(struct.pack(">B", length))
        out.write(data)

    def cmd_copy(pos, length):
        if pos >= 2**16 and length >= 2**16:
            out.write(struct.pack(">BII", CMD_COPY_POS32_LEN32, pos, length))
        elif pos >= 2**16 and length >= 2**8:
            out.write(struct.pack(">BIH", CMD_COPY_POS32_LEN16, pos, length))
        elif pos >= 2**16:
            out.write(struct.pack(">BIB", CMD_COPY_POS32_LEN8, pos, length))
        elif length >= 2**16:
            out.write(struct.pack(">BHI", CMD_COPY_POS16_LEN32, pos, length))
        elif length >= 2**8:
            out.write(struct.pack(">BHH", CMD_COPY_POS16_LEN16, pos, length))
        else:
            out.write(struct.pack(">BHB", CMD_COPY_POS16_LEN8, pos, length))

    data_pos = 0
    data_len = 0

    while offset < len(b):
        match_pos = 0
        match_len = 0

        # Check for a copy large enough to be more efficient
        for check_len in range(7, len(b) - offset):
            data = b[offset:offset+check_len]
            try:
                index = a.index(data)
                match_len = check_len
                match_pos = index
            except ValueError:
                break

        if match_len > 0:
            cmd_data(data_pos, data_len)
            cmd_copy(match_pos, match_len)
            offset += match_len
            data_pos = offset
            data_len = 0

        offset += 1
        data_len += 1

    # Add any remaining data
    cmd_data(data_pos, data_len)

    out.write(struct.pack(">B", CMD_EOF))

def main():
    if len(sys.argv) < 4:
        print("Usage: diff.py <from> <to> <output>", file=sys.stderr)
        sys.exit(1)

    from_path = sys.argv[1]
    to_path = sys.argv[2]
    out_path = sys.argv[3]

    with open(from_path, "rb") as f:
        a = f.read()

    with open(to_path, "rb") as f:
        b = f.read()

    with open(out_path, "wb") as f:
        make_diff(a, b, f)


if __name__ == "__main__":
    main()
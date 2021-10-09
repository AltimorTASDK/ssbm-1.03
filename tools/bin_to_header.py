import os
import sys
import textwrap
from itertools import takewhile

def rle_encode(data):
    i = 0
    out = bytearray()
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

def main():
    if len(sys.argv) < 3:
        print("Usage: bin_to_header.py <in> <out>", file=sys.stderr)
        sys.exit(1)
        
    in_path = sys.argv[1]
    out_path = sys.argv[2]
        
    with open(in_path, "rb") as f:
        data = f.read()
        
    var_name = os.path.basename(in_path).replace(".", "_") + "_data"
        
    encoded = rle_encode(data)
        
    data_text = textwrap.wrap(", ".join([f"0x{b:02X}" for b in encoded]), 100,
                              initial_indent="\t", subsequent_indent="\t")
        
    with open(out_path, "w") as f:
        f.write(
            "#pragma once\n"
            "\n"
            f"inline unsigned char {var_name}[] = {{\n"
            + "\n".join(data_text) +
            "\n};\n")

if __name__ == "__main__":
    main()
import os
import sys
import textwrap

def main():
    if len(sys.argv) < 3:
        print("Usage: bin_to_header.py <in> <out>", file=sys.stderr)
        sys.exit(1)
        
    in_path = sys.argv[1]
    out_path = sys.argv[2]
        
    with open(in_path, "rb") as f:
        data = f.read()
        
    var_name = os.path.basename(in_path).replace(".", "_") + "_data"
        
    data_text = textwrap.wrap(", ".join([f"0x{b:02X}" for b in data]), 100,
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
import struct
import sys
from datetime import datetime

BLOCK_SIZE = 0x2000

def gamecube_time():
    delta = datetime.today() - datetime(2000, 1, 1)
    return int(delta.total_seconds())

def write_gci_header(file, filename, block_count):
    file.write(b"GALE01")                          # Game + maker code
    file.write(struct.pack(">B", 0xFF))            # Reserved
    file.write(struct.pack(">B", 0))               # Banner flags
    file.write(filename.ljust(0x20, b"\x00"))      # Filename
    file.write(struct.pack(">I", gamecube_time())) # Last modification time
    file.write(struct.pack(">I", 0))               # Image data offset
    file.write(struct.pack(">H", 0))               # Icon format
    file.write(struct.pack(">H", 0))               # Animation speed
    file.write(struct.pack(">B", 0))               # File permissions
    file.write(struct.pack(">B", 0))               # Copy counter
    file.write(struct.pack(">H", 0))               # First block
    file.write(struct.pack(">H", block_count))     # Block count
    file.write(struct.pack(">H", 0xFFFF))          # Reserved
    file.write(struct.pack(">I", 0))               # Comments address

def main():
    if len(sys.argv) < 4:
        print("Usage: bin_to_gci.py <in> <out> <gci filename>", file=sys.stderr)
        sys.exit(1)

    in_path = sys.argv[1]
    out_path = sys.argv[2]
    filename = sys.argv[3]

    with open(in_path, "rb") as f:
        data = f.read()

    block_count = (len(data) + BLOCK_SIZE - 1) // BLOCK_SIZE

    # Pad to block size
    data = data.ljust(block_count * BLOCK_SIZE, b"\x00")

    with open(out_path, "wb") as f:
        write_gci_header(f, filename.encode(), block_count)
        f.write(data)

if __name__ == "__main__":
    main()
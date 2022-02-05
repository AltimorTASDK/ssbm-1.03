import struct
import sys
from datetime import datetime

BLOCK_SIZE = 0x2000

FILENAME_SIZE = 0x20
TITLE_SIZE    = 0x40

CARD_ATTRIB_PUBLIC = 0x04
CARD_ATTRIB_NOCOPY = 0x08
CARD_ATTRIB_NOMOVE = 0x10

CARD_BANNER_NONE = 0x00
CARD_BANNER_CI   = 0x01
CARD_BANNER_RGB  = 0x02

def gamecube_time():
    delta = datetime.today() - datetime(2000, 1, 1)
    return int(delta.total_seconds())

def write_gci_header(file, filename, block_count, banner_offset):
    file.write(b"GALE01")                             # Game + maker code
    file.write(struct.pack(">B", 0xFF))               # Reserved
    file.write(struct.pack(">B", CARD_BANNER_RGB))    # Banner flags
    file.write(filename.ljust(FILENAME_SIZE, b'\0'))  # Filename
    file.write(struct.pack(">I", gamecube_time()))    # Last modification time
    file.write(struct.pack(">I", banner_offset))      # Image data offset
    file.write(struct.pack(">H", 0))                  # Icon format
    file.write(struct.pack(">H", 0))                  # Animation speed
    file.write(struct.pack(">B", CARD_ATTRIB_PUBLIC)) # File permissions
    file.write(struct.pack(">B", 0))                  # Copy counter
    file.write(struct.pack(">H", 0))                  # First block
    file.write(struct.pack(">H", block_count))        # Block count
    file.write(struct.pack(">H", 0xFFFF))             # Reserved
    file.write(struct.pack(">I", 0))                  # Comments address

def main():
    if len(sys.argv) < 4:
        print("Usage: bin_to_gci.py <out> <filename> <title> <banner file> <input files...>",
              file=sys.stderr)
        sys.exit(1)

    out_path = sys.argv[1]
    filename = sys.argv[2]
    title    = sys.argv[3]
    banner   = sys.argv[4]
    in_files = sys.argv[5:]

    if len(filename) > FILENAME_SIZE:
        print("Filename is too long", file=sys.stderr)
        sys.exit(1)

    if len(title) > TITLE_SIZE:
        print("Title is too long", file=sys.stderr)
        sys.exit(1)

    data = title.encode().ljust(TITLE_SIZE, b'\0')

    for path in in_files:
        with open(path, "rb") as f:
            in_data = f.read()
            data += struct.pack(">I", len(in_data)) + in_data

    if len(banner) != 0:
        banner_offset = len(data)
        with open(banner, "rb") as f:
            f.seek(6) # Skip tex header
            data += f.read()
    else:
        banner_offset = -1

    block_count = (len(data) + BLOCK_SIZE - 1) // BLOCK_SIZE

    # Pad to block size
    data = data.ljust(block_count * BLOCK_SIZE, b'\0')

    with open(out_path, "wb") as f:
        write_gci_header(f, filename.encode(), block_count, banner_offset)
        f.write(data)

if __name__ == "__main__":
    main()
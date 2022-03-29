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

CARD_SPEED_END    = 0x00
CARD_SPEED_FAST   = 0x01
CARD_SPEED_MIDDLE = 0x02
CARD_SPEED_SLOW   = 0x03

GX_TF_RGB5A3 = 5
GX_TF_CI8    = 9

TEX_HEADER_SIZE = 6

def gamecube_time():
    delta = datetime.today() - datetime(2000, 1, 1)
    return int(delta.total_seconds())

def write_gci_header(file, filename, block_count, banner_offset, banner_flags,
                     icon_flags):
    if icon_flags != CARD_BANNER_NONE:
        animation_speed = CARD_SPEED_MIDDLE
    else:
        animation_speed = CARD_SPEED_END

    file.write(b"GALE01")                             # Game + maker code
    file.write(struct.pack(">B", 0xFF))               # Reserved
    file.write(struct.pack(">B", banner_flags))       # Banner flags
    file.write(filename.ljust(FILENAME_SIZE, b'\0'))  # Filename
    file.write(struct.pack(">I", gamecube_time()))    # Last modification time
    file.write(struct.pack(">I", banner_offset))      # Image data offset
    file.write(struct.pack(">H", icon_flags))         # Icon format
    file.write(struct.pack(">H", animation_speed))    # Animation speed
    file.write(struct.pack(">B", CARD_ATTRIB_PUBLIC)) # File permissions
    file.write(struct.pack(">B", 0))                  # Copy counter
    file.write(struct.pack(">H", 0))                  # First block
    file.write(struct.pack(">H", block_count))        # Block count
    file.write(struct.pack(">H", 0xFFFF))             # Reserved
    file.write(struct.pack(">I", 0))                  # Comments address

def get_image_flags_from_header(header):
    _, _, fmt = struct.unpack(">HHBx", header)
    if fmt == GX_TF_RGB5A3:
        return CARD_BANNER_RGB
    elif fmt == GX_TF_CI8:
        return CARD_BANNER_CI
    else:
        print("Unsupported image format", file=sys.stderr)
        sys.exit(1)

def main():
    if len(sys.argv) < 4:
        print("Usage: bin_to_gci.py <out> <filename> <title> <banner file> <icon file> <input files...>",
              file=sys.stderr)
        sys.exit(1)

    out_path = sys.argv[1]
    filename = sys.argv[2]
    title    = sys.argv[3]
    banner   = sys.argv[4]
    icon     = sys.argv[5]
    in_files = sys.argv[6:]

    if len(filename) > FILENAME_SIZE:
        print("Filename is too long", file=sys.stderr)
        sys.exit(1)

    if len(title) > TITLE_SIZE:
        print("Title is too long", file=sys.stderr)
        sys.exit(1)

    data = title.encode().ljust(TITLE_SIZE, b'\0')

    for path in in_files:
        try:
            with open(path, "rb") as f:
                # Write size of each file followed by contents
                in_data = f.read()
                data += struct.pack(">I", len(in_data)) + in_data
        except IOError:
            print(f"Warning: Unable to open input file {path}. Skipping.",
                  file=sys.stderr)

    # Mark EOF with 0 size
    data += struct.pack(">I", 0)

    if len(banner) != 0:
        banner_offset = len(data)
        with open(banner, "rb") as f:
            header = f.read(TEX_HEADER_SIZE)
            banner_flags = get_image_flags_from_header(header)
            data += f.read()

        if len(icon) != 0:
            with open(icon, "rb") as f:
                header = f.read(TEX_HEADER_SIZE)
                icon_flags = get_image_flags_from_header(header)
                data += f.read()
    else:
        banner_offset = -1
        banner_flags  = CARD_BANNER_NONE
        icon_flags    = CARD_BANNER_NONE

    block_count = (len(data) + BLOCK_SIZE - 1) // BLOCK_SIZE

    # Pad to block size
    data = data.ljust(block_count * BLOCK_SIZE, b'\0')

    with open(out_path, "wb") as f:
        write_gci_header(f, filename.encode(), block_count, banner_offset,
                         banner_flags, icon_flags)
        f.write(data)

if __name__ == "__main__":
    main()
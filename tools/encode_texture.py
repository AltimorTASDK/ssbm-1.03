import math
import os
import png
import struct
import sys
from itertools import product

GX_TF_I4     = 0
GX_TF_IA4    = 2
GX_TF_RGB5A3 = 5
GX_TF_RGBA8  = 6

RED   = 0
GREEN = 1
BLUE  = 2
ALPHA = 3

FORMAT_DICT = {
    'i4': GX_TF_I4,
    'ia4': GX_TF_IA4,
    'rgb5a3': GX_TF_RGB5A3,
    'rgba8': GX_TF_RGBA8
}

def block_range(x, y, step=(1, 1)):
    x = math.ceil(x / step[0]) * step[0]
    y = math.ceil(y / step[1]) * step[1]
    ranges = (range(0, y, step[1]), range(0, x, step[0]))
    return ((x, y) for y, x in product(*ranges))

def encode_i4(out_file, get_pixel, width, height):
    """
    Write 8x8 pixel blocks with 4bpp
    """
    BLOCK_SIZE = (8, 8)

    for block_x , block_y in block_range(width, height, BLOCK_SIZE):
        for offset_x, offset_y in block_range(*BLOCK_SIZE, (2, 1)):
            x = block_x + offset_x
            y = block_y + offset_y
            a = get_pixel(x,     y, RED) >> 4
            b = get_pixel(x + 1, y, RED) >> 4
            out_file.write(bytes([(a << 4) | b]))

def encode_ia4(out_file, get_pixel, width, height):
    """
    Write 8x4 pixel blocks with lower nibble as intensity, upper nibble as alpha
    Use red channel as intensity
    """
    BLOCK_SIZE = (8, 4)

    for block_x , block_y in block_range(width, height, BLOCK_SIZE):
        for offset_x, offset_y in block_range(*BLOCK_SIZE):
            x = block_x + offset_x
            y = block_y + offset_y
            red   = get_pixel(x, y, RED)   >> 4
            alpha = get_pixel(x, y, ALPHA) >> 4
            out_file.write(bytes([(alpha << 4) | red]))

def encode_rgb5a3(out_file, get_pixel, width, height):
    """
    Write 4x4 pixel blocks with pixels encoded differently if they're opaque:
    0AAARRRRGGGGBBBB Translucent
    1RRRRRGGGGGBBBBB Opaque
    """
    BLOCK_SIZE = (4, 4)

    for block_x, block_y in block_range(width, height, BLOCK_SIZE):
        for offset_x, offset_y in block_range(*BLOCK_SIZE):
            x = block_x + offset_x
            y = block_y + offset_y
            alpha = get_pixel(x, y, ALPHA) >> 5
            if alpha == 7:
                # Opaque
                red   = get_pixel(x, y, RED)   >> 3
                green = get_pixel(x, y, GREEN) >> 3
                blue  = get_pixel(x, y, BLUE)  >> 3
                value = (1 << 15) | (red << 10) | (green << 5) | blue
            else:
                # Translucent
                red   = get_pixel(x, y, RED)   >> 4
                green = get_pixel(x, y, GREEN) >> 4
                blue  = get_pixel(x, y, BLUE)  >> 4
                value = (alpha << 12) | (red << 8) | (green << 4) | blue

            out_file.write(bytes([value >> 8, value & 0xFF]))
    
def encode_rgba8(out_file, get_pixel, width, height):
    """
    Write in 4x4 pixel blocks of format:
    ARARARARARARARAR
    ARARARARARARARAR
    GBGBGBGBGBGBGBGB
    GBGBGBGBGBGBGBGB
    """
    BLOCK_SIZE = (4, 4)

    for block_x, block_y in block_range(width, height, BLOCK_SIZE):
        for offset_x, offset_y in block_range(*BLOCK_SIZE):
            x = block_x + offset_x
            y = block_y + offset_y
            out_file.write(bytes([get_pixel(x, y, ALPHA)]))
            out_file.write(bytes([get_pixel(x, y, RED)]))

        for offset_x, offset_y in block_range(*BLOCK_SIZE):
            x = block_x + offset_x
            y = block_y + offset_y
            out_file.write(bytes([get_pixel(x, y, GREEN)]))
            out_file.write(bytes([get_pixel(x, y, BLUE)]))

def encode_png(in_path, out_path, fmt):
    with open(in_path, "rb") as in_file:
        reader = png.Reader(file=open(in_path, "rb"))
        width, height, [*data], info = reader.asRGBA8()
        
    with open(out_path, "wb") as out_file:
        def get_pixel(x, y, channel):
            if x >= width or y >= height:
                return 0
            return data[y][x * 4 + channel]
        
        { # Call corresponding encoding function
            GX_TF_I4:     encode_i4,
            GX_TF_IA4:    encode_ia4,
            GX_TF_RGB5A3: encode_rgb5a3,
            GX_TF_RGBA8:  encode_rgba8
        }[fmt](out_file, get_pixel, width, height)
        

def main():
    if len(sys.argv) < 3:
        print("Usage: encode_texture.py <in> <out>", file=sys.stderr)
        sys.exit(1)

    in_path = sys.argv[1]
    out_path = sys.argv[2]
    out_dir = os.path.dirname(out_path)

    if not os.path.exists(out_dir):
        os.makedirs(out_dir)
        
    # Get format name e.g. texture.rgba8.png, default RGBA8
    name = os.path.basename(in_path)
    components = name.split(".")
    if len(components) == 3:
        fmt = FORMAT_DICT.get(components[-2], GX_TF_RGBA8)
    else:
        fmt = GX_TF_RGBA8

    encode_png(in_path, out_path, fmt)

if __name__ == "__main__":
    main()
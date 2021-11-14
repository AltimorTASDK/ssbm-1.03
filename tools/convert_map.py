import sys

SECTION_OFFSET_START = 0x0
SECTION_ADDRESS_START = 0x48
SECTION_SIZE_START = 0x90

BSS_START = 0xD8
BSS_SIZE = 0xDC

TEXT_SECTION_COUNT = 7
DATA_SECTION_COUNT = 11
SECTION_COUNT = TEXT_SECTION_COUNT + DATA_SECTION_COUNT

def word(data, offset):
    return sum(data[offset + i] << (24 - i * 8) for i in range(4))

def word_to_bytes(word):
    return bytes((word >> (24 - i * 8)) & 0xFF for i in range(4))

def get_address_section(data, value):
    for i in range(0, SECTION_COUNT):
        address = word(data, SECTION_ADDRESS_START + i * 4)
        size = word(data, SECTION_SIZE_START + i * 4)
        if address <= value < address + size:
            return i

def address_to_offset(data, value):
    for i in range(0, SECTION_COUNT):
        address = word(data, SECTION_ADDRESS_START + i * 4)
        size = word(data, SECTION_SIZE_START + i * 4)
        if address <= value < address + size:
            offset = word(data, SECTION_OFFSET_START + i * 4)
            return value - address + offset

def offset_to_address(data, value):
    for i in range(0, SECTION_COUNT):
        offset = word(data, SECTION_OFFSET_START + i * 4)
        size = word(data, SECTION_SIZE_START + i * 4)
        if offset <= value < offset + size:
            address = word(data, SECTION_ADDRESS_START + i * 4)
            return value - offset + address

def get_match_count(data1, data2):
    return len([1 for x, y in zip(data1, data2) if x == y])

last_offset = 0

def match_address(address, size, in_dol, out_dol, func_list):
    global last_offset

    offset = address_to_offset(in_dol, address)
    if offset is None:
        return

    pattern = in_dol[offset:offset+size]

    best_matches = []
    best_match_count = 0

    for scan_address in func_list:
        scan_offset = address_to_offset(out_dol, scan_address)
        candidate = out_dol[scan_offset:scan_offset+size]
        match_count = get_match_count(pattern, candidate)
        if match_count > best_match_count:
            best_matches = [scan_address]
            best_match_count = match_count
        elif match_count == best_match_count:
            best_matches.append(scan_address)

    if len(best_matches) == 1:
        match = best_matches[0]
        print(f"{address:08X} -> {match:08X} (offset {match - address:08X})")
    elif (address + last_offset) in best_matches:
        match = address + last_offset
        print(f"{address:08X} -> {match:08X} (offset {match - address:08X})")
    else:
        #for match in best_matches:
        #    print(f"{address:08X} -> {match:08X} (offset {match - address:08X})")
        match = None

    if len(best_matches) == 1:
        last_offset = best_matches[0] - address

    return match

def parse_map(in_dol, in_map, out_dol, out_map, func_list):
    for line in in_map:
        if line.startswith("."):
            continue

        address, size, _, _, name, *_ = line.split()
        if name.startswith("zz_"):
            continue

        address = int(address, 16)
        size = int(size, 16)

        match = match_address(address, size, in_dol, out_dol, func_list)
        if match is not None:
            out_map.write(f"{match:08X} {size:08X} {match:08X} 0 {name}\n")

def main():
    if len(sys.argv) < 6:
        print("Usage: convert_map.py <in dol> <in map> <out dol> <out map> <out function list>",
              file=sys.stderr)
        sys.exit(1)

    in_dol_path = sys.argv[1]
    in_map_path = sys.argv[2]
    out_dol_path = sys.argv[3]
    out_map_path = sys.argv[4]
    out_func_list_path = sys.argv[5]

    with open(in_dol_path, "rb") as f:
        in_dol = f.read()

    with open(out_dol_path, "rb") as f:
        out_dol = f.read()

    with open(out_func_list_path, "r") as f:
        func_list = [int(line, 16) for line in f]

    with open(in_map_path, "r") as in_map:
        with open(out_map_path, "w") as out_map:
            parse_map(in_dol, in_map, out_dol, out_map, func_list)

if __name__ == "__main__":
    main()
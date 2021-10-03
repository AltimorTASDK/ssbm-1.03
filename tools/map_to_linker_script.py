def is_char_allowed(c):
    return 'a' <= c <= 'z' or 'A' <= c <= 'Z' or '0' <= c <= '9' or c == '_'

def main():
    script = ""
    with open("GALE01.map") as f:
        while True:
            line = f.readline()
            if not line:
                break
            if line.startswith("."):
                continue

            address, _, _, _, name, *_ = line.split()
            if name.startswith("zz_"):
                continue

            name = "".join(c for c in name if is_char_allowed(c))
            # Remove leading underscores
            #while name.startswith("_"):
            #    name = name[1:]

            script += f"{name:60s} = 0x{address};\n"

    with open("GALE01.ld", "w") as f:
        f.write(script)

if __name__ == "__main__":
    main()
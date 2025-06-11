#!/usr/bin/env python3
import sys
import os

def rotr8(val, r):
    return ((val & 0xFF) >> r) | ((val & 0xFF) << (8 - r) & 0xFF)

def encrypt(data, key):
    out = bytearray(len(data))
    klen = len(key)
    for i, b in enumerate(data):
        b = (~b) & 0xFF
        b = rotr8(b, 3)
        b ^= key[i % klen]
        out[i] = b
    return out

def load_key(arg):
    if os.path.isfile(arg):
        with open(arg, 'r') as f:
            text = f.read()
        tokens = [tok for tok in text.replace(',', ' ').split()]
    else:
        tokens = arg.split(',')
    try:
        key = bytes(int(x, 16) for x in tokens)
    except ValueError:
        print("Error: Invalid hex value in key.")
        sys.exit(1)
    return key

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: encrypt_exe.py <exe_path> <key_hex_or_file> <output_c>")
        sys.exit(1)

    exe_path, key_arg, out_c = sys.argv[1], sys.argv[2], sys.argv[3]

    if not os.path.isfile(exe_path):
        print(f"Error: EXE file '{exe_path}' does not exist.")
        sys.exit(1)

    key = load_key(key_arg)

    with open(exe_path, "rb") as f:
        data = f.read()

    enc = encrypt(data, key)

    with open(out_c, "w") as f:
        f.write(f"// payload size: {len(enc)} bytes\n")
        f.write(f"constexpr size_t PAYLOAD_SIZE = {len(enc)};\n")
        f.write("BYTE encryptedPayload[PAYLOAD_SIZE] = {\n")
        for i in range(0, len(enc), 16):
            chunk = enc[i:i + 16]
            line = ", ".join(f"0x{b:02X}" for b in chunk)
            f.write("    " + line)
            if i + 16 < len(enc):
                f.write(",\n")
        f.write("\n};\n")

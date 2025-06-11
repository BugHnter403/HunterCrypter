# generate_key_hex.py
import random

# Generate 16 random bytes
key = [random.randint(0, 255) for _ in range(16)]

# Format each byte as two-digit hex and join with commas
key_hex = ','.join(f'{b:02X}' for b in key)

# Save to key.hex
with open('key.hex', 'w') as f:
    f.write(key_hex)

print("key.hex created with contents:")
print(key_hex)
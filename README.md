
![Capture331](https://github.com/user-attachments/assets/f7de3e44-d31e-408a-892b-eadd96948873)

![Capture112w](https://github.com/user-attachments/assets/7f6a2c81-5351-40f0-a9a8-a48464c700ff)

https://www.virustotal.com/gui/file/29267ecfb8fef5e303810996552b4fddc11a8bda8904db19afe6858a9b7f3d58/detection



# 🕵️‍♂️ StealthLoader Crypter (FUD Runtime EXE Encryptor)

This project provides a full implementation of a runtime EXE crypter. It includes:

- A C++ loader (`StealthLoader.cpp`) that decrypts and executes a payload in memory.
- Python tools to:
  - 🔐 Generate a random XOR+bitshift key
  - 🧬 Encrypt any EXE/shellcode file into a C++ `BYTE[]` array

---

## 📁 Project Structure

/Crypter/
│

├── encrypt.py # Encrypts a .exe file to a C++ array

├── generate_key_hex.py # Generates a random encryption key (hex)

├── key.hex # Key generated from the above script

├── payload.cpp # Output of encryption: C++-formatted payload

├── StealthLoader/

│ └── StealthLoader.cpp # The main crypter loader in C++


---

## ✅ Step-by-Step Usage

### 1. 🔑 Generate Random Key

Use the following script to generate a 16-byte random encryption key:

```bash
python HexGen.py

This will:

Print a key like 7F,3A,22,D5,...

Save it to a file called key.hex

2. 🔐 Encrypt Your EXE File
Use the encryption script to encrypt any .exe (or shellcode) using the key:


python encrypt_exe.py <input_exe> <key_file_or_inline> <output_cpp>

Example:

python encrypt_exe.py Test.exe key.hex payload.cpp

This generates:

payload.cpp containing encryptedPayload[] and PAYLOAD_SIZE.

3. ⚙️ Paste Encrypted Output Into C++ Loader
Open your StealthLoader.cpp file.

Copy-paste the contents of payload.cpp into the top.

Also paste your key in the same format:

BYTE key[16] = {
    0x7F, 0x3A, 0x22, 0xD5, 0x91, 0x12, 0x88, 0x6C,
    0xAD, 0x4E, 0x2C, 0x1B, 0xF0, 0x99, 0x73, 0x00
};
✅ Make sure:
PAYLOAD_SIZE is correct

Key size is 16 (or match what was used)

4. 🧪 Build the Project
Open the StealthLoader project in Visual Studio.

Set to Release mode, x64.

Build the project.

5. 🧨 Run the Loader
When you execute the compiled loader, it:

Decrypts the payload in memory

Executes it directly without writing to disk

❓ How the Encryption Works
The encrypt_exe.py script:

Inverts bits (~b)

Performs ROTR8 (right rotate 8-bit)

XORs each byte with a repeating key

This basic polymorphic transformation helps obfuscate payloads to evade detection.

            
###***⚠️ Disclaimer
This project was developed strictly for educational and research purposes. I do not endorse or condone the use of this tool for malicious activities. Any misuse of this code or concepts is not my responsibility. By using this repository, you agree to comply with all applicable laws and regulations in your jurisdiction.

Use responsibly. 🔒***###

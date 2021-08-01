#include "../main.h"
#include "armhook.h"
#include <sys/mman.h>

uintptr_t mmap_start    = 0;
uintptr_t mmap_end      = 0;
uintptr_t memlib_start  = 0;
uintptr_t memlib_end    = 0;

uintptr_t ARMHook::getLibraryAddress(const char* library)
{
    char filename[0xFF] = {0},
    buffer[2048] = {0};
    FILE *fp = 0;
    uintptr_t address = 0;

    sprintf( filename, "/proc/%d/maps", getpid() );

    fp = fopen( filename, "rt" );
    if(fp == 0)
    {
        LOGI("ERROR: can't open file %s", filename);
        goto done;
    }

    while(fgets(buffer, sizeof(buffer), fp))
    {
        if( strstr( buffer, library ) )
        {
            address = (uintptr_t)strtoul( buffer, 0, 16 );
            break;
        }
    }

    done:

    if(fp) {
      fclose(fp);
    }

    return address;
}

void ARMHook::sa_initializeTrampolines(uintptr_t start, uintptr_t end)
{
    memlib_start = start;
    memlib_end = memlib_start + end;

    mmap_start = (uintptr_t)mmap(0, PAGE_SIZE, PROT_WRITE | PROT_READ | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    mprotect((void*)(mmap_start & 0xFFFFF000), PAGE_SIZE, PROT_READ | PROT_EXEC | PROT_WRITE);
    mmap_end = (mmap_start + PAGE_SIZE);
}

void ARMHook::unprotect(uintptr_t ptr)
{
    mprotect((void*)(ptr & 0xFFFFF000), PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC);
}

void ARMHook::makeNOP(uintptr_t addr, unsigned int count)
{
    ARMHook::unprotect(addr);

    for(uintptr_t ptr = addr; ptr != (addr+(count*2)); ptr += 2)
    {
        *(char*)ptr = 0x00;
        *(char*)(ptr+1) = 0x46;
    }

    cacheflush(addr, (uintptr_t)(addr + count*2), 0);
}

void ARMHook::writeMemory(uintptr_t dest, uintptr_t src, size_t size)
{
    // FLog("writeMemory addr: 0x%X", dest);

    ARMHook::unprotect(dest);
    memcpy((void*)dest, (void*)src, size);
    cacheflush(dest, dest+size, 0);
}

void ARMHook::readMemory(uintptr_t dest, uintptr_t src, size_t size)
{
    ARMHook::unprotect(src);
    memcpy((void*)dest, (void*)src, size);
}

void ARMHook::JMPCode(uintptr_t func, uintptr_t addr)
{
    uint32_t code = ((addr-func-4) >> 12) & 0x7FF | 0xF000 | ((((addr-func-4) >> 1) & 0x7FF | 0xB800) << 16);
    ARMHook::writeMemory(func, (uintptr_t)&code, 4);
}

void ARMHook::WriteHookProc(uintptr_t addr, uintptr_t func)
{
    char code[16];
    memcpy(code, HOOK_PROC, 16);
    *(uint32_t*)&code[12] = (func | 1);

    ARMHook::writeMemory(addr, (uintptr_t)code, 16);
}

void ARMHook::installMethodHook(uintptr_t addr, uintptr_t func)
{
    ARMHook::unprotect(addr);
    *(uintptr_t*)addr = func;
}

void ARMHook::installPLTHook(uintptr_t addr, uintptr_t func, uintptr_t *orig)
{
    ARMHook::unprotect(addr);
    *orig = *(uintptr_t*)addr;
    *(uintptr_t*)addr = func;
}

void ARMHook::InjectCode(uintptr_t addr, uintptr_t func, int reg)
{
    char injectCode[12];

    injectCode[0] = 0x01;
    injectCode[1] = 0xA0 + reg;
    injectCode[2] = (0x08 * reg) + reg;
    injectCode[3] = 0x68;
    injectCode[4] = 0x87 + (0x08 * reg);
    injectCode[5] = 0x46;
    injectCode[6] = injectCode[4];
    injectCode[7] = injectCode[5];
    
    *(uintptr_t*)&injectCode[8] = func;

    ARMHook::writeMemory(addr, (uintptr_t)injectCode, 12);
}

void ARMHook::makeRET(uintptr_t addr)
{
    ARMHook::writeMemory(addr, (uintptr_t)"\x00\x20\xF7\x46", 4);
}

void SetupGameHook(uintptr_t addr, uintptr_t func, uintptr_t *orig)
{
    // FLog("SetUpHook: 0x%X -> 0x%X", addr, func);

    if(memlib_end < (memlib_start + 0x10) || mmap_end < (mmap_start + 0x20))
    {
        FLog("[!] code #435");
        std::terminate();
    }

    ARMHook::readMemory(mmap_start, addr, 4);
    ARMHook::WriteHookProc(mmap_start+4, addr+4);
    *orig = mmap_start+1;
    mmap_start += 32;

    ARMHook::JMPCode(addr, memlib_start);
    ARMHook::WriteHookProc(memlib_start, func);
    memlib_start += 16;
}
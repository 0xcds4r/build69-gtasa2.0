#pragma once

#define HOOK_PROC "\x01\xB4\x01\xB4\x01\x48\x01\x90\x01\xBD\x00\xBF\x00\x00\x00\x00"

void SetupGameHook(uintptr_t addr, uintptr_t func, uintptr_t *orig);

class ARMHook
{
public:
	static uintptr_t getLibraryAddress(const char* library);
	static void sa_initializeTrampolines(uintptr_t start, uintptr_t end);
	static void unprotect(uintptr_t ptr);
	static void makeNOP(uintptr_t addr, unsigned int count);
	static void writeMemory(uintptr_t dest, uintptr_t src, size_t size);
	static void readMemory(uintptr_t dest, uintptr_t src, size_t size);
	static void JMPCode(uintptr_t func, uintptr_t addr);
	static void WriteHookProc(uintptr_t addr, uintptr_t func);
	static void installMethodHook(uintptr_t addr, uintptr_t func);
	static void InjectCode(uintptr_t addr, uintptr_t func, int reg);
	static void makeRET(uintptr_t addr);
	static void installHook(uintptr_t addr, uintptr_t func, uintptr_t *orig, bool gta);
	static void installPLTHook(uintptr_t addr, uintptr_t func, uintptr_t *orig);
};
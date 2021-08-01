#include <jni.h>
#include <android/log.h>
#include <ucontext.h>
#include <pthread.h>
#include <dlfcn.h>

#include "main.h"
#include "game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "chatwindow.h"
#include "keyboard.h"
#include "dialog.h"
#include "spawnscreen.h"
#include "playertags.h"
#include "settings.h"
#include "debug.h"

#include "util/armhook.h"
#include "checkfilehash.h"
#include "str_obfuscator_no_template.hpp"

CGame *pGame = nullptr;
CNetGame *pNetGame = nullptr;
CChatWindow *pChatWindow = nullptr;
CSpawnScreen *pSpawnScreen = nullptr;
CPlayerTags *pPlayerTags = nullptr;
CDialogWindow *pDialogWindow = nullptr;

CGUI *pGUI = nullptr;
CKeyBoard *pKeyBoard = nullptr;
CDebug *pDebug = nullptr;
CSettings *pSettings = nullptr;

uintptr_t g_GTASAAdr = 0;
uintptr_t g_SCANDAdr = 0;

bool bSAMPInitialized = false;
bool bScreenUpdated = false;

bool bGameInited = false;
bool bNetworkInited = false;

void InitRenderWareFunctions();
void InstallGlobalHooks();
void ApplyGlobalPatches();
void ApplySCAndPatches();

void handler(int signum, siginfo_t *info, void* contextPtr)
{
	ucontext* context = (ucontext_t*)contextPtr;

	if(info->si_signo == SIGSEGV)
	{
		FLog("backtrace:");
		FLog("1: libGTASA.so + 0x%X", context->uc_mcontext.arm_pc - g_GTASAAdr);
		FLog("2: libGTASA.so + 0x%X", context->uc_mcontext.arm_lr - g_GTASAAdr);
		FLog("3: libsamp.so + 0x%X", context->uc_mcontext.arm_pc - ARMHook::getLibraryAddress("libsamp.so"));
		FLog("4: libsamp.so + 0x%X", context->uc_mcontext.arm_lr - ARMHook::getLibraryAddress("libsamp.so"));

		exit(0);
	}

	return;
}

void InitialiseInterfaces()
{
	if(!pKeyBoard)
	{
		pKeyBoard = new CKeyBoard();
	}
		
	if(!pChatWindow)
	{
		pChatWindow = new CChatWindow();
	}

	#ifndef DEBUG_MODE
	{
		if(!pSpawnScreen)
		{
			pSpawnScreen = new CSpawnScreen();
		}
	
		if(!pPlayerTags)
		{
			pPlayerTags = new CPlayerTags();
		}
			
		if(!pDialogWindow)
		{
			pDialogWindow = new CDialogWindow();
		}
	}
	#endif
}

void DoInitStuff()
{
	if(!bGameInited)
	{
		pGame->Initialise();
		pGame->SetMaxStats();
		pGame->ToggleThePassingOfTime(0);

		InitialiseInterfaces();
			
		#ifdef DEBUG_MODE
		{
			pDebug = new CDebug();

			if(pDebug) {
				pDebug->SpawnLocalPlayer();
			}
		}
		#endif

		bGameInited = true;
		
		return;
	}

	#ifndef DEBUG_MODE
	{
		if(!bNetworkInited)
		{
			if(!pNetGame)
			{
				pNetGame = new CNetGame(cryptor::create(SRV_IP, MAX_IP_LENGTH).decrypt(), 7777, "Artem_Ldev", nullptr);
			}
			
			bNetworkInited = true;
			
			return;
		}
	}
	#endif
}

void MainLoop()
{
	DoInitStuff();
}

void InitGUI()
{
	pGUI = new CGUI();
}

void TryInitialiseSAMP()
{
	if(!bSAMPInitialized)
	{
		// StartGameScreen::OnNewGameCheck
		(( int (*)())(g_GTASAAdr+0x2A7201))();

		bSAMPInitialized = true;
	}
}

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
	__android_log_write(ANDROID_LOG_INFO, "AXL", "SA-MP Library loaded! Build time: " __DATE__ " " __TIME__);

	g_GTASAAdr = ARMHook::getLibraryAddress("libGTASA.so");
	g_SCANDAdr = ARMHook::getLibraryAddress("libSCAnd.so");

	if(g_GTASAAdr)
	{
		if(g_SCANDAdr)
		{
			ARMHook::sa_initializeTrampolines(g_GTASAAdr+0x180044, 0x800);

			ApplyGlobalPatches();
			InstallGlobalHooks();
			InitRenderWareFunctions();
			ApplySCAndPatches();
			
			pGame = new CGame();

			struct sigaction act;
			act.sa_sigaction = handler;
			sigemptyset(&act.sa_mask);
			act.sa_flags = SA_SIGINFO;
			sigaction(SIGSEGV, &act, 0);

			return JNI_VERSION_1_4;
		}
	}	

	return 0;
}

void LOGI(const char *fmt, ...)
{	
	char buffer[0xFF];

	memset(buffer, 0, sizeof(buffer));

	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, arg);
	va_end(arg);

	__android_log_write(ANDROID_LOG_INFO, "AXL", buffer);

	return;
}

void FLog(const char *fmt, ...)
{	
	const char* g_pszStorage = (const char*)(g_GTASAAdr+0x6D687C);
	if(!g_pszStorage || !strlen(g_pszStorage)) {
		return;
	}

	char buffer[0xFF];
	static FILE* flLog = nullptr;

	if(flLog == nullptr && g_pszStorage != nullptr)
	{
		sprintf(buffer, "%sSAMP/client.log", g_pszStorage);
		flLog = fopen(buffer, "a");
	}

	memset(buffer, 0, sizeof(buffer));

	va_list arg;
	va_start(arg, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, arg);
	va_end(arg);

	__android_log_write(ANDROID_LOG_INFO, "AXL", buffer);

	if(flLog == nullptr) return;
	fprintf(flLog, "%s\n", buffer);
	fflush(flLog);

	return;
}

uint32_t GetTickCount()
{
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	return (tv.tv_sec*1000+tv.tv_usec/1000);
}

const char* GetGameStorage()
{
	return (const char*)(g_GTASAAdr+0x6D687C);
}
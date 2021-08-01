#include "../main.h"
#include "../util/armhook.h"
#include "RW/RenderWare.h"
#include "game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "chatwindow.h"

extern CNetGame *pNetGame;
extern CGUI *pGUI;
extern CChatWindow *pChatWindow;
extern CGame *pGame;

void InitGUI();
void TryInitialiseSAMP();
void MainLoop();
void HookCPad();

extern bool bGameInited;
extern bool bNetworkInited;

bool bLoopStarted = false;

typedef struct _PED_MODEL
{
	uintptr_t 	vtable;
	uint8_t		data[88];
} PED_MODEL; // SIZE = 92

PED_MODEL PedsModels[315];
int PedsModelsCount = 0;

extern "C" uintptr_t get_lib() 
{
 	return g_GTASAAdr;
}

// ----------------------------------------------------------------------------------------------------

struct stFile
{
	int isFileExist;
	FILE *f;
};

stFile* (*OS_FileOpen)(int a1, unsigned int a2, char *path, int a4);
stFile* OS_FileOpen_hook(int a1, unsigned int a2, char *path, int a4)
{
	char path_new[0xFF] = { 0 };

	const char* g_pszStorage = (const char*)(g_GTASAAdr+0x6D687C);

	if ( !strncmp(path, "data\\script\\mainV1.scm", 0x16) )
  	{
  		sprintf(path_new, "%sSAMP/main.scm", g_pszStorage);
  		goto orig;
  	}

  	if ( !strncmp(path, "DATA\\SCRIPT\\SCRIPTV1.IMG", 0x18) )
  	{
  		sprintf(path_new, "%sSAMP/script.img", g_pszStorage);
  		goto orig;
  	}

  	if ( !strncmp(path, "DATA\\GTA.DAT", 0xC) )
  	{
  		sprintf(path_new, "%sSAMP/gta.dat", g_pszStorage);
  		goto orig;
  	}

  	if ( !strncmp(path, "DATA\\PEDS.IDE", 0xD) )
  	{
  		sprintf(path_new, "%sSAMP/peds.ide", g_pszStorage);
  		goto orig;
  	}

  	if ( !strncmp(path, "DATA\\TIMECYC.DAT", 0x10) )
  	{
  		sprintf(path_new, "%sSAMP/timecyc.dat", g_pszStorage);
  		goto orig;
  	}

  	if ( !strncmp(path, "data\\paths\\tracks2.dat", 0x15) )
  	{
  		sprintf(path_new, "%sSAMP/tracks2.dat", g_pszStorage);
  		goto orig;
  	}

  	if ( !strncmp(path, "data\\paths\\tracks4.dat", 0x15) )
  	{
  		sprintf(path_new, "%sSAMP/tracks4.dat", g_pszStorage);
  		goto orig;
  	}

orig:
	return OS_FileOpen(a1, a2, path, a4);

open:
	stFile *st = (stFile*)malloc(8);
	st->isFileExist = false;

	FILE *f  = fopen(path, "rb");
	if(f)
	{
		st->isFileExist = true;
		st->f = f;
		return st;
	}
	else
	{
		free(st);
		st = nullptr;
		return 0;
	}
}

void (*CStream__InitImageList)();
void CStream__InitImageList_Hook()
{
	// fuck
	ARMHook::unprotect(g_GTASAAdr + 0x792DA8);
	ARMHook::unprotect(g_GTASAAdr + 0x792DA4);
	ARMHook::unprotect(g_GTASAAdr + 0x792DD4);
	ARMHook::unprotect(g_GTASAAdr + 0x792DD8);
	ARMHook::unprotect(g_GTASAAdr + 0x792E04);
	ARMHook::unprotect(g_GTASAAdr + 0x792E08);
	ARMHook::unprotect(g_GTASAAdr + 0x792E34);
	ARMHook::unprotect(g_GTASAAdr + 0x792E38);
	ARMHook::unprotect(g_GTASAAdr + 0x792E64);
	ARMHook::unprotect(g_GTASAAdr + 0x792E68);
	ARMHook::unprotect(g_GTASAAdr + 0x792E94);
	ARMHook::unprotect(g_GTASAAdr + 0x792E98);
	ARMHook::unprotect(g_GTASAAdr + 0x792EC4);
	ARMHook::unprotect(g_GTASAAdr + 0x792EC8);
	ARMHook::unprotect(g_GTASAAdr + 0x792EF4);
	ARMHook::unprotect(g_GTASAAdr + 0x792D78);

  	*(uint8_t *)(g_GTASAAdr + 0x792DA8) = 0;
  	*(uint32_t *)(g_GTASAAdr + 0x792DA4) = 0;
  	*(uint32_t *)(g_GTASAAdr + 0x792DD4) = 0;
  	*(uint8_t *)(g_GTASAAdr + 0x792DD8) = 0;
  	*(uint32_t *)(g_GTASAAdr + 0x792E04) = 0;
  	*(uint8_t *)(g_GTASAAdr + 0x792E08) = 0;
  	*(uint32_t *)(g_GTASAAdr + 0x792E34) = 0;
  	*(uint8_t *)(g_GTASAAdr + 0x792E38) = 0;
  	*(uint32_t *)(g_GTASAAdr + 0x792E64) = 0;
  	*(uint8_t *)(g_GTASAAdr + 0x792E68) = 0;
  	*(uint32_t *)(g_GTASAAdr + 0x792E94) = 0;
  	*(uint8_t *)(g_GTASAAdr + 0x792E98) = 0;
  	*(uint32_t *)(g_GTASAAdr + 0x792EC4) = 0;
  	*(uint8_t *)(g_GTASAAdr + 0x792EC8) = 0;
  	*(uint32_t *)(g_GTASAAdr + 0x792EF4) = 0;
  	*(uint8_t *)(g_GTASAAdr + 0x792D78) = 0;

	// CStreaming::AddImageToList
	((uint32_t(*)(char*, uint32_t))(g_GTASAAdr + 0x2CF761))("TEXDB\\GTA3.IMG", 1);
	((uint32_t(*)(char*, uint32_t))(g_GTASAAdr + 0x2CF761))("TEXDB\\GTA_INT.IMG", 1);
	// ((uint32_t(*)(char*, uint32_t))(g_GTASAAdr + 0x2CF761))("TEXDB\\SAMP.IMG", 1);
	// ((uint32_t(*)(char*, uint32_t))(g_GTASAAdr + 0x2CF761))("TEXDB\\SAMPCOL.IMG", 1);
}

void (*CGame__InitialiseRenderWare)();
void CGame__InitialiseRenderWare_Hook()
{
	// FLog("CGame__InitialiseRenderWare_Hook");

	CGame__InitialiseRenderWare();

	// TextureDatabaseRuntime::Load()
	// ((void (*)(const char*, int, int))(g_GTASAAdr + 0x1EA8E5))("samp", 0, 5);

	// Initialise GUI
	InitGUI();
	
	return;
}

PED_MODEL* (*CModelInfo__AddPedModel)(int id);
PED_MODEL* CModelInfo__AddPedModel_Hook(int id)
{
	// FLog("CModelInfo__AddPedModel_Hook");

	PED_MODEL* model = &PedsModels[PedsModelsCount];
	memset(model, 0, sizeof(PED_MODEL));
	model->vtable = (uintptr_t)(g_GTASAAdr + 0x667668);

	// CClumpModelInfo::CClumpModelInit()
	((uintptr_t(*)(PED_MODEL*))(*(void**)(model->vtable + 0x1C)))(model);

	*(PED_MODEL**)(g_GTASAAdr + 0x91DCB8 + (id * 4)) = model; // CModelInfo::ms_modelInfoPtrs

	PedsModelsCount++;
	return model;
}

void (*Render2dStuff)();
void Render2dStuff_Hook()
{
	Render2dStuff();
	MainLoop();

	bLoopStarted = true;

	if(bGameInited)
	{
		if(bNetworkInited)
		{
			if(pNetGame) 
			{
				/*uint32_t pVal = *(uint32_t*)(g_GTASAAdr+0x96B510);
    			if(pChatWindow) {
        			pChatWindow->AddDebugMessage("value: %d", pVal);
   		 		}*/

				pNetGame->Process();
			}
		}

		if(pGUI) {
			pGUI->Render();
		}
	}
	
	return;
}

void (*Render2dStuffAfterFade)();
void Render2dStuffAfterFade_Hook()
{
	Render2dStuffAfterFade();
}

void (*TouchEvent)(int, int, int posX, int posY);
void TouchEvent_Hook(int type, int num, int posX, int posY)
{
	if(pChatWindow && posX > 0 && posY > 0) {
		// pChatWindow->AddDebugMessage("x: %d y: %d", posX, posY);
	}

	if(bGameInited && pGUI && (pGUI->OnTouchEvent(type, num, posX, posY)) == true) {
		return TouchEvent(type, num, posX, posY);
	}
}

void (*MainMenuScreen__Update)(uintptr_t thiz, float unk);
void MainMenuScreen__Update_Hook(uintptr_t thiz, float unk)
{
	TryInitialiseSAMP();
	return MainMenuScreen__Update(thiz, unk);
}

void InstallSAMPHooks()
{
	SetupGameHook(g_GTASAAdr + 0x2697C0, (uintptr_t)TouchEvent_Hook, (uintptr_t*)&TouchEvent);
}

bool NotifyEnterVehicle(VEHICLE_TYPE *_pVehicle)
{
    FLog("NotifyEnterVehicle");
 	
    if(!pNetGame) {
    	return false;
    } 
    
    CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
    if(!pVehiclePool) {
    	return false;
    }

    CVehicle *pVehicle = nullptr;
    VEHICLEID VehicleID = pVehiclePool->FindIDFromGtaPtr(_pVehicle);

    if(VehicleID <= 0 || VehicleID >= MAX_VEHICLES) {
    	return false;
    }

    if(!pVehiclePool->GetSlotState(VehicleID)) {
    	return false;
    }

    pVehicle = pVehiclePool->GetAt(VehicleID);
    if(!pVehicle) {
    	return false;
    }
 
    CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
 	
 	if(pLocalPlayer) {
 		FLog("Vehicle ID: %d", VehicleID);
    	pLocalPlayer->SendEnterVehicleNotification(VehicleID, false);
 	}
 
    return true;
}

extern "C" bool TaskEnterVehicle(uintptr_t pVehicle, uintptr_t a2)
{
	FLog("TaskEnterVehicle");

	if(!NotifyEnterVehicle((VEHICLE_TYPE*)pVehicle)) {
		return false;
	}

	// CTask::operator new
	uintptr_t pTask = ((uintptr_t (*)(void))(g_GTASAAdr + 0x4D6A01))(); 

	// CTaskComplexEnterCarAsDriver::CTaskComplexEnterCarAsDriver
  	((void (__fastcall *)(uintptr_t, uintptr_t))(g_GTASAAdr + 0x4F6F71))(pTask, pVehicle); 

  	// CTaskManager::SetTask
  	((int (__fastcall *)(uintptr_t, uintptr_t, int, int))(g_GTASAAdr + 0x53390B))(a2, pTask, 3, 0); 

	return true;
}

void __attribute__((naked)) TaskEnterVehicle_hook(uintptr_t thiz, uintptr_t pVehicle)
{
	// 2.0
	__asm__ volatile("push {r1-r11, lr}\n\t"
		"mov r0, r8\n\t"
		"adds r1, r6, #4\n\t"
		"blx TaskEnterVehicle\n\t"
		"pop {r1-r11, lr}\n\t"
		"blx get_lib\n\t"
		"add r0, #0x400000\n\t"
        "add r0, #0xAC00\n\t"
        "add r0, #0x41\n\t"
        "mov pc, r0\n\t");
}

void (*CTaskComplexLeaveCar)(uintptr_t** thiz, VEHICLE_TYPE *pVehicle, int iTargetDoor, int iDelayTime, bool bSensibleLeaveCar, bool bForceGetOut);
void CTaskComplexLeaveCar_Hook(uintptr_t** thiz, VEHICLE_TYPE *pVehicle, int iTargetDoor, int iDelayTime, bool bSensibleLeaveCar, bool bForceGetOut) 
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_GTASAAdr;

	if(dwRetAddr == 0x40A819 || dwRetAddr == 0x409A43) 
	{
		if(pNetGame) 
		{
			if(*(uint32_t*)((uintptr_t)GamePool_FindPlayerPed() + 0x590) == (uint32_t)pVehicle) 
			{
				CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
				uint16_t VehicleID = pVehiclePool->FindIDFromGtaPtr(*(VEHICLE_TYPE**)((uintptr_t)GamePool_FindPlayerPed() + 0x590));
				CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();

				if(pLocalPlayer) {
					pLocalPlayer->SendExitVehicleNotification(VehicleID);
				}
			}
		}
	}
 
 	(*CTaskComplexLeaveCar)(thiz, pVehicle, iTargetDoor, iDelayTime, bSensibleLeaveCar, bForceGetOut);
}

int (*CTouchInterface__IsTouchDown)(uintptr_t thiz, uintptr_t touch);
int CTouchInterface__IsTouchDown_Hook(uintptr_t thiz, uintptr_t touch)
{
	if(thiz == 1 && touch == 2)
	{
		// enter vehicle
	}

	return CTouchInterface__IsTouchDown(thiz, touch);
}

void (*CGame__Process)();
void CGame__Process_Hook()
{
	CGame__Process();
}

uint16_t gxt_string[0x7F];
uint16_t* CText__Get_Hook(uintptr_t thiz, const char* text)
{
	if (text[0] == 'L' && text[1] == 'G' && text[2] == '_')
	{
		const char* code = &text[3];
		if (!strcmp(code, "LG_")) {
			CFont::AsciiToGxtChar("LG_05", gxt_string);
		}

		return gxt_string;
	}

	return ((uint16_t*(*)(uintptr_t, const char*))(g_GTASAAdr + 0x54DB8C+1))(thiz, text);
}

uint32_t (*CHudColours__GetIntColour)(uintptr_t thiz);
uint32_t CHudColours__GetIntColour_Hook(uintptr_t thiz)
{
	return TranslateColorCodeToRGBA(thiz);
}

void (*CWorld__ProcessPedsAfterPreRender)();
void CWorld__ProcessPedsAfterPreRender_Hook()
{
	CWorld__ProcessPedsAfterPreRender();

	if(pNetGame)
	{
		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();

		if(pPlayerPool)
		{
			pPlayerPool->ProcessAttachedObjects();
		}
	}
}

uint32_t (*CheckIfNewEntityNeedsScript)(uintptr_t thiz, int a2, int a3);
uint32_t CheckIfNewEntityNeedsScript_Hook(uintptr_t thiz, int a2, int a3)
{
	if(thiz && a2) {
		return CheckIfNewEntityNeedsScript(thiz, a2, a3);
	}

	return 0;
}

bool (*CPad__ExitVehicleJustDown)(uintptr_t thiz, int a2, uintptr_t vehicle, int a4, uintptr_t vec);
bool CPad__ExitVehicleJustDown_Hook(uintptr_t thiz, int a2, uintptr_t vehicle, int a4, uintptr_t vec)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_GTASAAdr;

	if(!vehicle) {
		return false;
	}

	bool bResult = CPad__ExitVehicleJustDown(thiz, a2, vehicle, a4, vec);

	// 40AB80
	if(bResult && dwRetAddr >= 0x40AB70 && dwRetAddr <= 0x40AB90) 
	{
		FLog("CPad__ExitVehicleJustDown: task enter vehicle");
		
		if(!NotifyEnterVehicle((VEHICLE_TYPE*)vehicle)) {
			return false;
		}
	}

	return bResult;
}

uint32_t dwParam1, dwParam2;
extern "C" void pickup_pickedup()
{
	if(pNetGame && pNetGame->GetPickupPool())
	{
		CPickupPool *pPickups = pNetGame->GetPickupPool();
		pPickups->PickedUp( ((dwParam1-(g_GTASAAdr+0x70E264))/0x20) );
	}
}

extern "C" void pickup_orig()
{
	ARMHook::writeMemory(g_GTASAAdr + 0x31E08C, (uintptr_t)"\x9A\xF8\x1C\x00\x13\x28\x00\xF2\x8F\x80\x01\x21", 12);
}

__attribute__((naked)) void PickupPickUp_hook(int a1, uint32_t a2, uint32_t a3, uint32_t a4)
{
	// ---------------------------------------------------------------
	__asm__ volatile(".byte 0x00, 0xB5\n\t");  						
	__asm__ volatile(".byte 0x01, 0xB4\n\t");  						
	// ---------------------------------------------------------------
	__asm__ volatile("blx get_lib\n\t");  							
	// ---------------------------------------------------------------
	__asm__ volatile(".byte 0x00, 0xF5, 0x44, 0x10\n\t");  			
	__asm__ volatile(".byte 0x00, 0xF5, 0x60, 0x40\n\t");  			
	__asm__ volatile(".byte 0x00, 0xF1, 0x8D, 0x00\n\t");  			
	__asm__ volatile(".byte 0x01, 0x46\n\t");  						
	__asm__ volatile(".byte 0x01, 0xBC\n\t");  						
	__asm__ volatile(".byte 0x5D, 0xF8, 0x04, 0xEB\n\t");  				
	__asm__ volatile(".byte 0x02, 0xB4\n\t");  	
	// ---------------------------------------------------------------					
	__asm__ volatile("push {r0-r11, lr}\n\t"
					"mov %0, r11" : "=r" (dwParam1));
	// ---------------------------------------------------------------
	__asm__ volatile(".byte 0x51, 0x46\n\t");  						
	__asm__ volatile(".byte 0x78, 0x44\n\t");  						
	__asm__ volatile(".byte 0x00, 0x68\n\t");  						
	__asm__ volatile(".byte 0x01, 0x60\n\t");  		
	// ---------------------------------------------------------------				
	__asm__ volatile("blx pickup_pickedup\n\t"); 	
	__asm__ volatile("blx pickup_orig\n\t");  			
	// ---------------------------------------------------------------
	__asm__ volatile(".byte 0xBD, 0xE8, 0xFF, 0x4F\n\t");  		
	__asm__ volatile(".byte 0x00, 0xBD\n\t");  			
	// ---------------------------------------------------------------
	// What the fuck is this variable, what is the value? can cut this value as bytes?
	// or is it worth finding a similar function?
}

uint32_t (*CPickup_Update)(unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4);
uint32_t CPickup_Update_hook(unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4)
{
	ARMHook::InjectCode(g_GTASAAdr + 0x31E08C, (uintptr_t)PickupPickUp_hook, 1);
  	return CPickup_Update(a1, a2, a3, a4);
}

int (*CPlaceable_InitMatrixArray)(uintptr_t thiz);
int CPlaceable_InitMatrixArray_hook(uintptr_t thiz)
{
	return ((int (__fastcall *)(int, signed int))(g_GTASAAdr + 0x407F85))(g_GTASAAdr + 0x95A988, 10000);
}

void InstallGlobalHooks()
{
	FLog("InstallGlobalHooks");

	ARMHook::installMethodHook(g_GTASAAdr + 0x66E78C, (uintptr_t)CText__Get_Hook);

	ARMHook::installPLTHook(g_GTASAAdr + 0x6625E0, (uintptr_t)MainMenuScreen__Update_Hook, (uintptr_t*)&MainMenuScreen__Update);
	ARMHook::installPLTHook(g_GTASAAdr + 0x67589C, (uintptr_t)Render2dStuff_Hook, (uintptr_t*)&Render2dStuff);
	
	ARMHook::installPLTHook(g_GTASAAdr + 0x674C68, (uintptr_t)CStream__InitImageList_Hook, (uintptr_t*)&CStream__InitImageList); 
	ARMHook::installPLTHook(g_GTASAAdr + 0x66F2D0, (uintptr_t)CGame__InitialiseRenderWare_Hook, (uintptr_t*)&CGame__InitialiseRenderWare); 
	// ARMHook::installPLTHook(g_GTASAAdr + 0x675D98, (uintptr_t)CModelInfo__AddPedModel_Hook, (uintptr_t*)&CModelInfo__AddPedModel);
	ARMHook::installPLTHook(g_GTASAAdr + 0x673950, (uintptr_t)CHudColours__GetIntColour_Hook, (uintptr_t*)&CHudColours__GetIntColour);

	ARMHook::InjectCode(g_GTASAAdr + 0x40AC28, (uintptr_t)TaskEnterVehicle_hook, 0);
	SetupGameHook(g_GTASAAdr + 0x4F8904, (uintptr_t)CTaskComplexLeaveCar_Hook, (uintptr_t*)&CTaskComplexLeaveCar);

	ARMHook::installPLTHook(g_GTASAAdr + 0x675554, (uintptr_t)CPlaceable_InitMatrixArray_hook, (uintptr_t*)&CPlaceable_InitMatrixArray);

	InstallSAMPHooks();
	HookCPad();
}
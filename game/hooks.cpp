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

struct _ATOMIC_MODEL
{
	uintptr_t func_tbl;
	char data[56];
};
extern struct _ATOMIC_MODEL* ATOMIC_MODELS;

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

stFile* (*NvFOpen)(const char*, const char*, int, int);
stFile* NvFOpen__Hook(const char* r0, const char* r1, int r2, int r3)
{
	char path[0xFF] = { 0 };

	if (!strncmp(r1, "DATA/PEDS.IDE", 13))
	{
		sprintf(path, "%s/SAMP/peds.ide", (const char*)(g_GTASAAdr+0x6D687C));
		FLog("Loading peds.ide..");
		goto open;
	}
	// ----------------------------
	if (!strncmp(r1, "DATA/GTA.DAT", 12))
	{
		sprintf(path, "%s/SAMP/gta.dat", (const char*)(g_GTASAAdr+0x6D687C));
		FLog("Loading gta.dat..");
		goto open;
	}

orig:
	return NvFOpen(r0, r1, r2, r3);

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

/*void (*CFileMgr__Initialise)(uintptr_t thiz);
void CFileMgr__Initialise_Hook(uintptr_t thiz)
{
	uintptr_t zipStorage = ((uintptr_t(*)(const char*))(g_GTASAAdr + 0x26FE55))("/Android/data/com.rockstargames.gtasa/samp.obb");

	if(zipStorage) {
		((int (*)(uintptr_t))(g_GTASAAdr + 0x26FF71))(zipStorage);
	}
   
	return CFileMgr__Initialise(thiz);
}*/

void (*CStream__InitImageList)();
void CStream__InitImageList_Hook()
{
	// FLog("CStream__InitImageList_Hook");

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
	((uint32_t(*)(char*, uint32_t))(g_GTASAAdr + 0x2CF761))("TEXDB\\SAMP.IMG", 1);
	((uint32_t(*)(char*, uint32_t))(g_GTASAAdr + 0x2CF761))("TEXDB\\SAMPCOL.IMG", 1);
}

void (*CGame__InitialiseRenderWare)();
void CGame__InitialiseRenderWare_Hook()
{
	// FLog("CGame__InitialiseRenderWare_Hook");

	CGame__InitialiseRenderWare();

	// TextureDatabaseRuntime::Load()
	((void (*)(const char*, int, int))(g_GTASAAdr + 0x1EA8E5))("samp", 0, 5);

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

uintptr_t (*CModelInfo__AddAtomicModel)(int iModel);
uintptr_t CModelInfo__AddAtomicModel_Hook(int iModel)
{
	// FLog("CModelInfo__AddAtomicModel_Hook");

	uint32_t iCount = *(uint32_t*)(g_GTASAAdr + 0x820738);
	_ATOMIC_MODEL* model = &ATOMIC_MODELS[iCount];
	*(uint32_t*)(g_GTASAAdr + 0x820738) = iCount + 1;

	// CClumpModelInfo::CClumpModelInit()
	((void(*)(_ATOMIC_MODEL*))(*(uintptr_t*)(model->func_tbl + 0x1C)))(model);

	_ATOMIC_MODEL** ms_modelInfoPtrs = (_ATOMIC_MODEL**)(g_GTASAAdr + 0x91DCB8); // CModelInfo::ms_modelInfoPtrs
	ms_modelInfoPtrs[iModel] = model;
	return (uintptr_t)model;
}

void (*CPools__Initialise)();
void CPools__Initialise_Hook()
{
	// FLog("CPools__Initialise_Hook");

	struct PoolAllocator {

		struct Pool {
			void* objects;
			uint8_t* flags;
			uint32_t count;
			uint32_t top;
			uint32_t bInitialized;
		};
		static_assert(sizeof(Pool) == 0x14);

		static Pool* Allocate(size_t count, size_t size) {

			Pool* p = new Pool;

			p->objects = new char[size * count];
			p->flags = new uint8_t[count];
			p->count = count;
			p->top = 0xFFFFFFFF;
			p->bInitialized = 1;

			for (size_t i = 0; i < count; i++) {
				p->flags[i] |= 0x80;
				p->flags[i] &= 0x80;
			}

			return p;
		}
	};

	// 600000 / 75000 = 8
	static auto ms_pPtrNodeSingleLinkPool = PoolAllocator::Allocate(100000, 8);		// 75000
	// 72000 / 6000 = 12
	static auto ms_pPtrNodeDoubleLinkPool = PoolAllocator::Allocate(60000, 12);	// 6000
	// 10000 / 500 = 20
	static auto ms_pEntryInfoNodePool = PoolAllocator::Allocate(20000, 20);	// 500
	// 279440 / 140 = 1996
	static auto ms_pPedPool = PoolAllocator::Allocate(240, 1996);	// 140
	// 286440 / 110 = 2604
	static auto ms_pVehiclePool = PoolAllocator::Allocate(2000, 2604);	// 110
	// 840000 / 14000 = 60
	static auto ms_pBuildingPool = PoolAllocator::Allocate(20000, 60);	// 14000
	// 147000 / 350 = 420
	static auto ms_pObjectPool = PoolAllocator::Allocate(3000, 420);	// 350
	// 210000 / 3500 = 60
	static auto ms_pDummyPool = PoolAllocator::Allocate(40000, 60);	// 3500
	// 487200 / 10150 = 48
	static auto ms_pColModelPool = PoolAllocator::Allocate(50000, 48);	// 10150
	// 64000 / 500 = 128
	static auto ms_pTaskPool = PoolAllocator::Allocate(5000, 128);	// 500
	// 13600 / 200 = 68
	static auto ms_pEventPool = PoolAllocator::Allocate(1000, 68);	// 200
	// 6400 / 64 = 100
	static auto ms_pPointRoutePool = PoolAllocator::Allocate(200, 100);	// 64
	// 13440 / 32 = 420
	static auto ms_pPatrolRoutePool = PoolAllocator::Allocate(200, 420);	// 32
	// 2304 / 64 = 36
	static auto ms_pNodeRoutePool = PoolAllocator::Allocate(200, 36);	// 64
	// 512 / 16 = 32
	static auto ms_pTaskAllocatorPool = PoolAllocator::Allocate(3000, 32);	// 16
	// 92960 / 140 = 664
	static auto ms_pPedIntelligencePool = PoolAllocator::Allocate(240, 664);	// 140
	// 15104 / 64 = 236
	static auto ms_pPedAttractorPool = PoolAllocator::Allocate(200, 236);	// 64

	*(PoolAllocator::Pool**)(g_GTASAAdr + 0x95AC38) = ms_pPtrNodeSingleLinkPool; // ms_pPtrNodeSingleLinkPool
	uintptr_t g_pPool = g_GTASAAdr + 0x95AC38;

	*(PoolAllocator::Pool**)(g_pPool + 4) = ms_pPtrNodeDoubleLinkPool; // ms_pPtrNodeDoubleLinkPool
	*(PoolAllocator::Pool**)(g_pPool + 8) = ms_pEntryInfoNodePool; // ms_pEntryInfoNodePool
	*(PoolAllocator::Pool**)(g_pPool + 12) = ms_pPedPool; // ms_pPedPool
	*(PoolAllocator::Pool**)(g_pPool + 16) = ms_pVehiclePool; // ms_pVehiclePool
	*(PoolAllocator::Pool**)(g_pPool + 20) = ms_pBuildingPool; // ms_pBuildingPool
	*(PoolAllocator::Pool**)(g_pPool + 24) = ms_pObjectPool; // ms_pObjectPool
	*(PoolAllocator::Pool**)(g_pPool + 28) = ms_pDummyPool; // ms_pDummyPool
	*(PoolAllocator::Pool**)(g_pPool + 32) = ms_pColModelPool; // ms_pColModelPool
	*(PoolAllocator::Pool**)(g_pPool + 36) = ms_pTaskPool; // ms_pTaskPool
	*(PoolAllocator::Pool**)(g_pPool + 40) = ms_pEventPool; // ms_pEventPool
	*(PoolAllocator::Pool**)(g_pPool + 44) = ms_pPointRoutePool; // ms_pPointRoutePool
	*(PoolAllocator::Pool**)(g_pPool + 48) = ms_pPatrolRoutePool; // ms_pPatrolRoutePool
	*(PoolAllocator::Pool**)(g_pPool + 52) = ms_pNodeRoutePool; // ms_pNodeRoutePool
	*(PoolAllocator::Pool**)(g_pPool + 56) = ms_pTaskAllocatorPool; // ms_pTaskAllocatorPool
	*(PoolAllocator::Pool**)(g_pPool + 60) = ms_pPedIntelligencePool; // ms_pPedIntelligencePool
	*(PoolAllocator::Pool**)(g_pPool + 64) = ms_pPedAttractorPool; // ms_pPedAttractorPool
}

void (*CPlaceable__InitMatrixArray)();
void CPlaceable__InitMatrixArray_Hook()
{
	// FLog("CPlaceable__InitMatrixArray_Hook");
	
	// CMatrixLinkList::Init
	reinterpret_cast<int(*)(unsigned long, unsigned long)>(g_GTASAAdr+0x407F85)(g_GTASAAdr+0x95A988, 10000);
}

int (*CCustomRoadsignMgr__RenderRoadsignAtomic)(uintptr_t thiz, uintptr_t a2);
int CCustomRoadsignMgr__RenderRoadsignAtomic_Hook(uintptr_t thiz, uintptr_t a2)
{
	// FLog("CCustomRoadsignMgr__RenderRoadsignAtomic_Hook");

	if(!thiz) {
		return 0;
	}

	return CCustomRoadsignMgr__RenderRoadsignAtomic(thiz, a2);
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
			if(pNetGame) {
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
	if(bGameInited && pGUI && (pGUI->OnTouchEvent(type, num, posX, posY)) == true) {
		return TouchEvent(type, num, posX, posY);
	}
}

extern "C" bool NotifyEnterVehicle(uintptr_t _pVehicle)
{
 	if(!_pVehicle) {
 		return false;
 	}

 	// todo

    return true;
}

void (*CTaskComplexEnterCarAsDriver)(uint32_t thiz, uint32_t pVehicle);
extern "C" void call_taskEnterCarAsDriver(uintptr_t a, uint32_t b)
{
	CTaskComplexEnterCarAsDriver(a, b);
}

void __attribute__((naked)) CTaskComplexEnterCarAsDriver_Hook(uint32_t thiz, uint32_t pVehicle)
{
	// todo: rewrite volatile to 2.0

    __asm__ volatile("push {r0-r11, lr}\n\t"
                    "mov r2, lr\n\t"
                    "blx get_lib\n\t"
                    "add r0, #0x3A0000\n\t"
                    "add r0, #0xEE00\n\t"
                    "add r0, #0xF7\n\t"
                    "cmp r2, r0\n\t"
                    "bne 1f\n\t" // !=
                    "mov r0, r1\n\t"
                    "blx NotifyEnterVehicle\n\t" // call NotifyEnterVehicle
                    "1:\n\t"  // call orig
                    "pop {r0-r11, lr}\n\t"
    				"push {r0-r11, lr}\n\t"
    				"blx call_taskEnterCarAsDriver\n\t"
    				"pop {r0-r11, pc}");
}

uint32_t dwParam1, dwParam2;
extern "C" void pickup_ololo()
{
	// 2.0
	if(pNetGame && pNetGame->GetPickupPool())
	{
		CPickupPool *pPickups = pNetGame->GetPickupPool();
		if(pPickups) {
			pPickups->PickedUp( ((dwParam1-(g_GTASAAdr+0x7AFD70))/0x20) );
		}
	}
}

__attribute__((naked)) void PickupPickUp_Hook()
{
	//LOGI("PickupPickUp_hook");

	// todo: rewrite volatile to 2.0
	//  0x12AD9E, v9 + 0x31E08D

	// calculate and save ret address
	__asm__ volatile("push {lr}\n\t"
					"push {r0}\n\t"
					"blx get_lib\n\t"
					"add r0, #0x2D0000\n\t"
					"add r0, #0x009A00\n\t"
					"add r0, #1\n\t"
					"mov r1, r0\n\t"
					"pop {r0}\n\t"
					"pop {lr}\n\t"
					"push {r1}\n\t");
	
	// 
	__asm__ volatile("push {r0-r11, lr}\n\t"
					"mov %0, r4" : "=r" (dwParam1));

	__asm__ volatile("blx pickup_ololo\n\t");


	__asm__ volatile("pop {r0-r11, lr}\n\t");

	// restore
	__asm__ volatile("ldrb r1, [r4, #0x1C]\n\t"
					"sub.w r2, r1, #0xD\n\t"
					"sub.w r2, r1, #8\n\t"
					"cmp r1, #6\n\t"
					"pop {pc}\n\t");
}

void (*CTaskComplexLeaveCar)(uintptr_t** thiz, uintptr_t pVehicle, int iTargetDoor, int iDelayTime, bool bSensibleLeaveCar, bool bForceGetOut);
void CTaskComplexLeaveCar_Hook(uintptr_t** thiz, uintptr_t pVehicle, int iTargetDoor, int iDelayTime, bool bSensibleLeaveCar, bool bForceGetOut) 
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_GTASAAdr;

	// 2.0
	if (dwRetAddr == 0x40A819 || dwRetAddr == 0x409A43) 
	{
		// todo
	}

	(*CTaskComplexLeaveCar)(thiz, pVehicle, iTargetDoor, iDelayTime, bSensibleLeaveCar, bForceGetOut);
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

void InstallCrashFixHooks()
{
	SetupGameHook(g_GTASAAdr + 0x5A60DC, (uintptr_t)CCustomRoadsignMgr__RenderRoadsignAtomic_Hook, (uintptr_t*)&CCustomRoadsignMgr__RenderRoadsignAtomic);
}

void (*CWidget__IsTouched)(uintptr_t thiz);
void CWidget__IsTouched_Hook(uintptr_t thiz)
{
	FLog("CWidget__IsTouched: 0x%x", thiz);
	if(pChatWindow) pChatWindow->AddDebugMessage("CWidget__IsTouched: 0x%x", thiz);
	CWidget__IsTouched(thiz);
}

void InstallGlobalHooks()
{
	FLog("InstallGlobalHooks");

	SetupGameHook(g_GTASAAdr + 0x29BFFC, (uintptr_t)MainMenuScreen__Update_Hook, (uintptr_t*)&MainMenuScreen__Update);
	SetupGameHook(g_GTASAAdr + 0x3F641C, (uintptr_t)Render2dStuff_Hook, (uintptr_t*)&Render2dStuff);
	SetupGameHook(g_GTASAAdr + 0x266E2C, (uintptr_t)NvFOpen__Hook, (uintptr_t*)&NvFOpen); 
	SetupGameHook(g_GTASAAdr + 0x2CF610, (uintptr_t)CStream__InitImageList_Hook, (uintptr_t*)&CStream__InitImageList); 
	SetupGameHook(g_GTASAAdr + 0x46F500, (uintptr_t)CGame__InitialiseRenderWare_Hook, (uintptr_t*)&CGame__InitialiseRenderWare); 
	SetupGameHook(g_GTASAAdr + 0x3860C4, (uintptr_t)CModelInfo__AddPedModel_Hook, (uintptr_t*)&CModelInfo__AddPedModel);
	SetupGameHook(g_GTASAAdr + 0x3859E8, (uintptr_t)CModelInfo__AddAtomicModel_Hook, (uintptr_t*)&CModelInfo__AddAtomicModel);
	SetupGameHook(g_GTASAAdr + 0x40C8B0, (uintptr_t)CPools__Initialise_Hook, (uintptr_t*)&CPools__Initialise);
	SetupGameHook(g_GTASAAdr + 0x408AD0, (uintptr_t)CPlaceable__InitMatrixArray_Hook, (uintptr_t*)&CPlaceable__InitMatrixArray);
	// SetupGameHook(g_GTASAAdr + 0x2B32B4, (uintptr_t)CWidget__IsTouched_Hook, (uintptr_t*)&CWidget__IsTouched);

	InstallCrashFixHooks();
	InstallSAMPHooks();
	HookCPad();
}
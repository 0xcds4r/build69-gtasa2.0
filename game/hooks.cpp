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

	// scm & img in apk

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

// bydlocode
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

int CCustomRoadsignMgr__RenderRoadsignAtomic_Hook(uintptr_t thiz, uintptr_t a2)
{
	// FLog("CCustomRoadsignMgr__RenderRoadsignAtomic_Hook");

	if(!thiz) {
		return 0;
	}

	// 0x5A60DC
	return ((int(*)(uintptr_t, uintptr_t))(g_GTASAAdr + 0x5A60DC+1))(thiz, a2);
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

int (*CUpsideDownCarCheck__IsCarUpsideDown)(int _thiz, uintptr_t _pVehicle);
int CUpsideDownCarCheck__IsCarUpsideDown_Hook(int _thiz, uintptr_t _pVehicle)
{
	if (*(uintptr_t*)(_pVehicle + 20) && _pVehicle != -1)
	{
		return CUpsideDownCarCheck__IsCarUpsideDown(_thiz, _pVehicle);
	}

	FLog("[x / Crash Preventer] CODE #1");

	return 0;
}

int (*RwFrameAddChild)(int _thiz, int _pData);
int RwFrameAddChild_Hook(int _thiz, int _pData)
{
	if(!_thiz || !_pData) {
		FLog("[x / Crash Preventer] CODE #2");
		return 0;
	} 

	return RwFrameAddChild(_thiz, _pData);
}

int (*RLEDecompress)(int _a1, unsigned int _a2, const char* _a3, unsigned int _a4, unsigned int _a5);
int RLEDecompress_Hook(int _a1, unsigned int _a2, const char* _a3, unsigned int _a4, unsigned int _a5)
{
	if (!_a3) {
		FLog("[x / Crash Preventer] CODE #3");
		return 0;
	}

	return RLEDecompress(_a1, _a2, _a3, _a4, _a5);
}

int (*CAnimBlendNode__FindKeyFrame)(uintptr_t _thiz, float _a2, int _a3, int _a4);
int CAnimBlendNode__FindKeyFrame_Hook(uintptr_t _thiz, float _a2, int _a3, int _a4)
{
	if (!_thiz || !*((uintptr_t*)_thiz + 4)) {
		FLog("[x / Crash Preventer] CODE #4");
		return 0;
	}

	return CAnimBlendNode__FindKeyFrame(_thiz, _a2, _a3, _a4);
}

int (*_rwFreeListFreeReal)(int a1, unsigned int a2);
int _rwFreeListFreeReal_Hook(int a1, unsigned int a2)
{
	if (a1 == 0 || !a1) {
		FLog("[x / Crash Preventer] CODE #5");
		return 0;
	}

	return _rwFreeListFreeReal(a1, a2);
}

int (*SetCompAlphaCB)(int a1, char a2);
int SetCompAlphaCB_Hook(int a1, char a2)
{
	if (!a1) {
		FLog("[x / Crash Preventer] CODE #6");
		return 0;
	}

	return SetCompAlphaCB(a1, a2);
}

int (*CAnimManager__UncompressAnimation)(uintptr_t _thiz, uintptr_t _a2);
int CAnimManager__UncompressAnimation_Hook(uintptr_t _thiz, uintptr_t _a2)
{
	if(!_thiz) {
		FLog("[x / Crash Preventer] CODE #7");
		return 0;
	}

	return CAnimManager__UncompressAnimation(_thiz, _a2);
}

void (*CStreaming__Init2)(uintptr_t thiz);
void CStreaming__Init2_Hook(uintptr_t thiz)
{
	CStreaming__Init2(thiz);

	// 685FA0 ; CStreaming::ms_memoryAvailable
	*(uint32_t*)(g_GTASAAdr + 0x685FA0) = 0x10000000;
}

char** (*CPhysical__Add)(uintptr_t _thiz);
char** CPhysical__Add_Hook(uintptr_t _thiz)
{
	if (pNetGame)
	{
		CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
		if (pPlayerPed)
		{
			for (size_t i = 0; i < 10; i++)
			{
				if (pPlayerPed->m_bObjectSlotUsed[i])
				{
					if ((uintptr_t)pPlayerPed->m_pAttachedObjects[i]->m_pEntity == _thiz)
					{
						CObject* pObject = pPlayerPed->m_pAttachedObjects[i];

						if (pObject->m_pEntity->mat->pos.X > 20000.0f || pObject->m_pEntity->mat->pos.Y > 20000.0f || pObject->m_pEntity->mat->pos.Z > 20000.0f ||
							pObject->m_pEntity->mat->pos.X < -20000.0f || pObject->m_pEntity->mat->pos.Y < -20000.0f || pObject->m_pEntity->mat->pos.Z < -20000.0f)
						{
							return 0;
						}
					}
				}
			}
		}

		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();

		if (pPlayerPool)
		{
			for (size_t i = 0; i < MAX_PLAYERS; i++)
			{
				if (pPlayerPool->GetSlotState(i))
				{
					CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(i);
					if (pRemotePlayer)
					{
						if (pRemotePlayer->GetPlayerPed() && pRemotePlayer->GetPlayerPed()->IsAdded())
						{
							pPlayerPed = pRemotePlayer->GetPlayerPed();

							// check attached object valid position
							for (size_t i = 0; i < 10; i++)
							{
								if (pPlayerPed->m_bObjectSlotUsed[i])
								{
									if ((uintptr_t)pPlayerPed->m_pAttachedObjects[i]->m_pEntity == _thiz)
									{
										CObject* pObject = pPlayerPed->m_pAttachedObjects[i];
										if (pObject->m_pEntity->mat->pos.X > 20000.0f || pObject->m_pEntity->mat->pos.Y > 20000.0f || pObject->m_pEntity->mat->pos.Z > 20000.0f ||
											pObject->m_pEntity->mat->pos.X < -20000.0f || pObject->m_pEntity->mat->pos.Y < -20000.0f || pObject->m_pEntity->mat->pos.Z < -20000.0f)
										{
											return 0;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return CPhysical__Add(_thiz);
}

void (*CPhysical__RemoveAndAdd)(uintptr_t _thiz);
void CPhysical__RemoveAndAdd_Hook(uintptr_t _thiz)
{
	if (pNetGame)
	{
		CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
		if (pPlayerPed)
		{
			for (size_t i = 0; i < 10; i++)
			{
				if (pPlayerPed->m_bObjectSlotUsed[i])
				{
					if ((uintptr_t)pPlayerPed->m_pAttachedObjects[i]->m_pEntity == _thiz)
					{
						CObject* pObject = pPlayerPed->m_pAttachedObjects[i];

						if (pObject->m_pEntity->mat->pos.X > 20000.0f || pObject->m_pEntity->mat->pos.Y > 20000.0f || pObject->m_pEntity->mat->pos.Z > 20000.0f ||
							pObject->m_pEntity->mat->pos.X < -20000.0f || pObject->m_pEntity->mat->pos.Y < -20000.0f || pObject->m_pEntity->mat->pos.Z < -20000.0f)
						{
							return;
						}
					}
				}
			}
		}

		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();

		if (pPlayerPool)
		{
			for (size_t i = 0; i < MAX_PLAYERS; i++)
			{
				if (pPlayerPool->GetSlotState(i))
				{
					CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(i);
					if (pRemotePlayer)
					{
						if (pRemotePlayer->GetPlayerPed() && pRemotePlayer->GetPlayerPed()->IsAdded())
						{
							pPlayerPed = pRemotePlayer->GetPlayerPed();

							// check attached object valid position
							for (size_t i = 0; i < 10; i++)
							{
								if (pPlayerPed->m_bObjectSlotUsed[i])
								{
									if ((uintptr_t)pPlayerPed->m_pAttachedObjects[i]->m_pEntity == _thiz)
									{
										CObject* pObject = pPlayerPed->m_pAttachedObjects[i];
										if (pObject->m_pEntity->mat->pos.X > 20000.0f || pObject->m_pEntity->mat->pos.Y > 20000.0f || pObject->m_pEntity->mat->pos.Z > 20000.0f ||
											pObject->m_pEntity->mat->pos.X < -20000.0f || pObject->m_pEntity->mat->pos.Y < -20000.0f || pObject->m_pEntity->mat->pos.Z < -20000.0f)
										{
											return;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return CPhysical__RemoveAndAdd(_thiz);
}

void InstallCrashFixHooks()
{
  	ARMHook::installPLTHook(g_GTASAAdr + 0x66EB0C, (uintptr_t)CUpsideDownCarCheck__IsCarUpsideDown_Hook, (uintptr_t*)&CUpsideDownCarCheck__IsCarUpsideDown); // 66EB0C
	ARMHook::installPLTHook(g_GTASAAdr + 0x675490, (uintptr_t)RwFrameAddChild_Hook, (uintptr_t*)&RwFrameAddChild); // 675490
	ARMHook::installPLTHook(g_GTASAAdr + 0x6701C8, (uintptr_t)RLEDecompress_Hook, (uintptr_t*)&RLEDecompress); // 6701C8
	ARMHook::installPLTHook(g_GTASAAdr + 0x67213C, (uintptr_t)CAnimBlendNode__FindKeyFrame_Hook, (uintptr_t*)&CAnimBlendNode__FindKeyFrame); // 67213C
	ARMHook::installPLTHook(g_GTASAAdr + 0x6787F0, (uintptr_t)_rwFreeListFreeReal_Hook, (uintptr_t*)&_rwFreeListFreeReal); // 6787F0
	ARMHook::installPLTHook(g_GTASAAdr + 0x6769F8, (uintptr_t)SetCompAlphaCB_Hook, (uintptr_t*)&SetCompAlphaCB); // 6769F8
	ARMHook::installPLTHook(g_GTASAAdr + 0x6750D4, (uintptr_t)CAnimManager__UncompressAnimation_Hook, (uintptr_t*)&CAnimManager__UncompressAnimation); // 6750D4
	ARMHook::installPLTHook(g_GTASAAdr + 0x6700D0, (uintptr_t)CStreaming__Init2_Hook, (uintptr_t*)&CStreaming__Init2); // 6700D0

	ARMHook::installPLTHook(g_GTASAAdr + 0x667CC4, (uintptr_t)CPhysical__Add_Hook, (uintptr_t*)&CPhysical__Add);
	ARMHook::installPLTHook(g_GTASAAdr + 0x66F4B0, (uintptr_t)CPhysical__RemoveAndAdd_Hook, (uintptr_t*)&CPhysical__RemoveAndAdd);
}

extern "C" bool NotifyEnterVehicle(VEHICLE_TYPE *_pVehicle)
{
    //Log("NotifyEnterVehicle");
 
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

    ENTITY_TYPE* g_pEntity = (ENTITY_TYPE*)&pVehicle->m_pVehicle->entity;
 	if(!g_pEntity) {
 		return false;
 	}

    if(*(uint16_t*)((uintptr_t)g_pEntity + 0x26) == TRAIN_PASSENGER) {
    	return false;
    }
 
 	uintptr_t pTemp = *(uintptr_t *)((uintptr_t)pVehicle->m_pVehicle + 1124); 
 	uint32_t dwPedType = *(uint32_t *)(pTemp + 1436);

    if(pVehicle->m_pVehicle->pDriver && dwPedType != 0) {
        return false;
    }
 
    CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
 	
 	if(pLocalPlayer) {
    	pLocalPlayer->SendEnterVehicleNotification(VehicleID, false);
 	}
 
    return true;
}

void (*CTaskComplexEnterCarAsDriver)(uint32_t thiz, uint32_t pVehicle);
extern "C" void call_taskEnterCarAsDriver(uintptr_t a, uint32_t b)
{
	CTaskComplexEnterCarAsDriver(a, b);
}

void __attribute__((naked)) CTaskComplexEnterCarAsDriver_hook(uint32_t thiz, uint32_t pVehicle)
{
	// todo: to 2.0
    __asm__ volatile("");
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
		// naaah. it's damn bullshit
	}

	return CTouchInterface__IsTouchDown(thiz, touch);
}

void (*CGame__Process)();
void CGame__Process_Hook()
{
	CGame__Process();

	if (pNetGame)
	{
		CTextDrawPool* pTextDrawPool = pNetGame->GetTextDrawPool();

		if (pTextDrawPool) {
			pTextDrawPool->SnapshotProcess();
		}
	}
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

uint32_t (*CHudColours__GetIntColour)(uintptr_t thiz, int a2);
uint32_t CHudColours__GetIntColour_Hook(uintptr_t thiz, int a2)
{
	return TranslateColorCodeToRGBA(a2);
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

void InstallGlobalHooks()
{
	FLog("InstallGlobalHooks");

	ARMHook::installMethodHook(g_GTASAAdr + 0x66E78C, (uintptr_t)CText__Get_Hook);

	ARMHook::installPLTHook(g_GTASAAdr + 0x6625E0, (uintptr_t)MainMenuScreen__Update_Hook, (uintptr_t*)&MainMenuScreen__Update);
	ARMHook::installPLTHook(g_GTASAAdr + 0x67589C, (uintptr_t)Render2dStuff_Hook, (uintptr_t*)&Render2dStuff);
	ARMHook::installPLTHook(g_GTASAAdr + 0x672448, (uintptr_t)NvFOpen__Hook, (uintptr_t*)&NvFOpen); 
	ARMHook::installPLTHook(g_GTASAAdr + 0x674C68, (uintptr_t)CStream__InitImageList_Hook, (uintptr_t*)&CStream__InitImageList); 
	ARMHook::installPLTHook(g_GTASAAdr + 0x66F2D0, (uintptr_t)CGame__InitialiseRenderWare_Hook, (uintptr_t*)&CGame__InitialiseRenderWare); 
	ARMHook::installPLTHook(g_GTASAAdr + 0x675D98, (uintptr_t)CModelInfo__AddPedModel_Hook, (uintptr_t*)&CModelInfo__AddPedModel);
	ARMHook::installPLTHook(g_GTASAAdr + 0x67579C, (uintptr_t)CModelInfo__AddAtomicModel_Hook, (uintptr_t*)&CModelInfo__AddAtomicModel);
	ARMHook::installPLTHook(g_GTASAAdr + 0x672468, (uintptr_t)CPools__Initialise_Hook, (uintptr_t*)&CPools__Initialise);
	ARMHook::installPLTHook(g_GTASAAdr + 0x675554, (uintptr_t)CPlaceable__InitMatrixArray_Hook, (uintptr_t*)&CPlaceable__InitMatrixArray);
	ARMHook::installPLTHook(g_GTASAAdr + 0x66FE58, (uintptr_t)CGame__Process_Hook, (uintptr_t*)&CGame__Process);
	ARMHook::installPLTHook(g_GTASAAdr + 0x673950, (uintptr_t)CHudColours__GetIntColour_Hook, (uintptr_t*)&CHudColours__GetIntColour);
	// ARMHook::installPLTHook(g_GTASAAdr + 0x675C68, (uintptr_t)CWorld__ProcessPedsAfterPreRender_Hook, (uintptr_t*)&CWorld__ProcessPedsAfterPreRender);

	// CTaskComplexEnterCarAsDriver
	// SetupGameHook(g_GTASAAdr + 0x4F6F70, (uintptr_t)CTaskComplexEnterCarAsDriver_Hook, (uintptr_t*)&CTaskComplexEnterCarAsDriver);

	// CTaskComplexLeaveCar::CTaskComplexLeaveCar
	SetupGameHook(g_GTASAAdr + 0x4F8904, (uintptr_t)CTaskComplexLeaveCar_Hook, (uintptr_t*)&CTaskComplexLeaveCar);

	// CTouchInterface__IsTouchDown
	// SetupGameHook(g_GTASAAdr + 0x2B0A38, (uintptr_t)CTouchInterface__IsTouchDown_Hook, (uintptr_t*)&CTouchInterface__IsTouchDown);

	InstallCrashFixHooks();
	InstallSAMPHooks();
	HookCPad();
}
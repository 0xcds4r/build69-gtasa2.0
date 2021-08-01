#include "../main.h"
#include "../util/armhook.h"
#include "common.h"

char* PLAYERS_REALLOC = nullptr;

struct _ATOMIC_MODEL
{
	uintptr_t func_tbl;
	char data[56];
};
struct _ATOMIC_MODEL *ATOMIC_MODELS;

uint8_t g_fps = 60;

void ApplyRadarPatches()
{
    FLog("ApplyRadarPatches");
    
    ARMHook::writeMemory(g_GTASAAdr + 0x4420D0, (uintptr_t)"\x2C\xE0", 2); // CRadar::Draw3dMarkers
    ARMHook::writeMemory(g_GTASAAdr + 0x44212C, (uintptr_t)"\x30\x46", 2); // CHudColours::GetIntColour
    ARMHook::writeMemory(g_GTASAAdr + 0x440470, (uintptr_t)"\x3A\xE0", 2); // CRadar::DrawEntityBlip
    ARMHook::writeMemory(g_GTASAAdr + 0x4404E8, (uintptr_t)"\x30\x46", 2); // CRadar::DrawEntityBlip 
    ARMHook::writeMemory(g_GTASAAdr + 0x43FB0E, (uintptr_t)"\x12\xE0", 2); // CRadar::DrawCoordBlip
    ARMHook::writeMemory(g_GTASAAdr + 0x43FB36, (uintptr_t)"\x48\x46", 2); // CRadar::DrawCoordBlip
    ARMHook::writeMemory(g_GTASAAdr + 0x2AB556, (uintptr_t)"\x00\x21", 2); // Menu_MapRender
}

void ApplyGlobalPatches()
{
	FLog("ApplyGlobalPatches");

    PLAYERS_REALLOC = (( char* (*)(int))(g_GTASAAdr + 0x198A70))(404 * 257 * sizeof(char));
    memset(PLAYERS_REALLOC, 0, 404 * 257);
    ARMHook::unprotect(g_GTASAAdr + 0x6783C8);
    *(char**)(g_GTASAAdr + 0x6783C8) = PLAYERS_REALLOC;

    // IMG count
    // ARMHook::writeMemory(g_GTASAAdr + 0x3F3648, (uintptr_t)"\x06\x20", 2); 

    // CBoat::ProcessControl
    ARMHook::makeNOP(g_GTASAAdr + 0x56C150, 4); 

     // CRealTimeShadowManager::ReturnRealTimeShadow ~ CPhysical
    ARMHook::makeNOP(g_GTASAAdr + 0x3FCD34, 2);

    // CPlaceable::InitMatrixArray
    // ARMHook::writeMemory(g_GTASAAdr + 0x408AE6, (uintptr_t)"\x4F\xF4\x7A\x61\x00", 4); 

    // CStats::GetFatAndMuscleModifier
    ARMHook::unprotect(g_GTASAAdr + 0x41C33C); 
    *(uint32_t *)(g_GTASAAdr + 0x41C33C) = 0x43300000;

    // CWidgetPlayerInfo::RenderArmorBar
    ARMHook::unprotect(g_GTASAAdr + 0x2BD94C); 
    *(uint32_t *)(g_GTASAAdr + 0x2BD94C) = 0x43300000; 

    // CEntryExit::GenerateAmbientPeds
    ARMHook::makeNOP(g_GTASAAdr + 0x3056D6, 4); 

    // blips
    ARMHook::makeNOP(g_GTASAAdr + 0x43FE0A, 2); 
    ARMHook::makeNOP(g_GTASAAdr + 0x44095E, 2); 
    ARMHook::makeNOP(g_GTASAAdr + 0x43FE08, 3); 
    ARMHook::makeNOP(g_GTASAAdr + 0x44095C, 3); 

    // CCamera::CamShake
    ARMHook::makeNOP(g_GTASAAdr + 0x5D87A6, 2);
    ARMHook::makeNOP(g_GTASAAdr + 0x5D8734, 2);
    // --------------------------------------------------------------------------------------------------

    ATOMIC_MODELS = new _ATOMIC_MODEL[20000];
    for (int i = 0; i < 20000; i++)
    {
      // CBaseModelInfo::CBaseModelInfo
      ((void(*)(_ATOMIC_MODEL*))(g_GTASAAdr + 0x384F89))(&ATOMIC_MODELS[i]);
      ATOMIC_MODELS[i].func_tbl = g_GTASAAdr + 0x667454;
      memset(ATOMIC_MODELS[i].data, 0, sizeof(ATOMIC_MODELS->data));
    }

    // _rwOpenGLRasterCreate
    ARMHook::writeMemory(g_GTASAAdr + 0x1AE95E, (uintptr_t)"\x01\x22", 2);
	
  	// CAudioEngine::StartLoadingTune
  	ARMHook::makeNOP(g_GTASAAdr + 0x5E4916, 2);
	 
    // Tasks
  	ARMHook::makeRET(g_GTASAAdr + 0x3976AC);	// CAEGlobalWeaponAudioEntity::ServiceAmbientGunFire
  	ARMHook::makeRET(g_GTASAAdr + 0x4211A0);	// CPlaceName::Process
  	ARMHook::makeRET(g_GTASAAdr + 0x538C8C);	// CTaskSimplePlayerOnFoot::PlayIdleAnimations
  	ARMHook::makeRET(g_GTASAAdr + 0x50AA58);	// CCarEnterExit::SetPedInCarDirect
  	   
    // generator
    ARMHook::makeRET(g_GTASAAdr + 0x56E350);  // CTheCarGenerators::Process
    ARMHook::makeRET(g_GTASAAdr + 0x4CF26C);  // CPopulation::AddPed

    // CPopulation::AddToPopulation
    // ARMHook::makeRET(g_GTASAAdr + 0x4CCA98);

  	ARMHook::makeRET(g_GTASAAdr + 0x2E82CC);  // CCarCtrl::GenerateRandomCars
    ARMHook::makeRET(g_GTASAAdr + 0x579214);  // CPlane::DoPlaneGenerationAndRemoval
    ARMHook::makeRET(g_GTASAAdr + 0x46B548);  // CFileLoader::LoadPickup
    ARMHook::makeRET(g_GTASAAdr + 0x306EC0);  // CEntryExit::GenerateAmbientPeds

    // hud patch
    ARMHook::makeRET(g_GTASAAdr + 0x436FCC);   // CHud::SetHelpMessageStatUpdate
    ARMHook::makeRET(g_GTASAAdr + 0x438634);  // CHud::DrawVehicleName
    ARMHook::makeRET(g_GTASAAdr + 0x436F5C); // CHud::SetHelpMessage

    // shadow patch
  	ARMHook::makeNOP(g_GTASAAdr + 0x3FCD34, 2);	   // ReturnRealTimeShadow
  	ARMHook::makeNOP(g_GTASAAdr + 0x3FCD74, 2);	  // ReturnRealTimeShadow
  	ARMHook::makeRET(g_GTASAAdr + 0x5B83FC);	 // CRealTimeShadowManager::Update

    // interior patch
    ARMHook::makeRET(g_GTASAAdr + 0x445E98); // Interior_c::AddPickups
    ARMHook::makeRET(g_GTASAAdr + 0x448984); // InteriorGroup_c::Exit
    ARMHook::makeRET(g_GTASAAdr + 0x40B028); // CPlayerInfo::FindObjectToSteal

    // player
    ARMHook::makeNOP(g_GTASAAdr + 0x40BED6, 2);  // CPlayerInfo::KillPlayer

    // CPed::RemoveWeaponWhenEnteringVehicle
  	ARMHook::makeNOP(g_GTASAAdr + 0x4A5328, 6);		
  
  	// fps debug
  	*(uint8_t *)(g_GTASAAdr + 0x98F1AD) = 1;

    // SaveGameForPause
    // ARMHook::makeRET(g_GTASAAdr + 0x5E4BE8);

    // CVehicleModelInfo::SetupCommonData ~ Increase matrix
    ARMHook::writeMemory(g_GTASAAdr + 0x468B7E, (uintptr_t)"\x4F\xF4\x00\x30", 4);
    ARMHook::writeMemory(g_GTASAAdr + 0x468B88, (uintptr_t)"\xF7\x20", 2);
    ARMHook::writeMemory(g_GTASAAdr + 0x468B8A, (uintptr_t)"\xF7\x25", 2);
    ARMHook::writeMemory(g_GTASAAdr + 0x468BCC, (uintptr_t)"\xF7\x28", 2);

    // CPlayerPed Task Patch
    ARMHook::writeMemory(g_GTASAAdr + 0x4C3673, (uintptr_t)"\xB3", 1);

    // CPlayerPed::ProcessAnimGroups
    ARMHook::makeNOP(g_GTASAAdr + 0x4C5EFA, 2);

    // CBike::ProcessAI
    ARMHook::makeNOP(g_GTASAAdr + 0x564CC0, 1); 

    // Vehicle Process Patch 
    ARMHook::makeNOP(g_GTASAAdr + 0x553E26, 2);
    ARMHook::makeNOP(g_GTASAAdr + 0x561A52, 2);
    ARMHook::makeNOP(g_GTASAAdr + 0x56BE64, 2);
    ARMHook::makeNOP(g_GTASAAdr + 0x57D054, 2);

    // blx CTaskComplexEnterCarAsDriver constructor in CPlayerInfo::Process (replace operator new task)
    ARMHook::writeMemory(g_GTASAAdr + 0x40AC28, (uintptr_t)"\x8F\xF5\x3A\xEF", 4); 
    ARMHook::makeNOP(g_GTASAAdr + 0x40AC30, 2); // CTaskComplexEnterCarAsDriver

    // CRunningScript::ProcessCommands300To399
    ARMHook::writeMemory(g_GTASAAdr + 0x341F34, (uintptr_t)"\x00\xF0\x21\xBE", 4);

    // psInitialize
    ARMHook::writeMemory(g_GTASAAdr + 0x5E4EC0, (uintptr_t)"\x00", 1);
    ARMHook::writeMemory(g_GTASAAdr + 0x5E4EC4, (uintptr_t)"\x00", 1);
    ARMHook::writeMemory(g_GTASAAdr + 0x5E4ECC, (uintptr_t)"\x00", 1);
    ARMHook::writeMemory(g_GTASAAdr + 0x5E4ED0, (uintptr_t)"\x00", 1);
    ARMHook::writeMemory(g_GTASAAdr + 0x5E4ED4, (uintptr_t)"\x00", 1);
    ARMHook::writeMemory(g_GTASAAdr + 0x5E4EAE, (uintptr_t)"\x00", 1);
    
    // CPlayerPed::DoesPlayerWantNewWeapon
    ARMHook::makeRET(g_GTASAAdr + 0x4C6708); 

    // Object pool
    ARMHook::writeMemory(g_GTASAAdr + 0x40CB40, (uintptr_t)"\x45\xF6\x00\x10\xC0\xF2\x06\x00", 8); // movw r0, #0x5900 | movt r0, #6
    ARMHook::writeMemory(g_GTASAAdr + 0x40CB4E, (uintptr_t)"\x4F\xF4\x3B\x60", 4); // MOV.W R0, #0xBB0
    ARMHook::writeMemory(g_GTASAAdr + 0x40CB52, (uintptr_t)"\x4F\xF4\x3B\x65", 4); // MOV.W R5, #0xBB0
    ARMHook::writeMemory(g_GTASAAdr + 0x40CB96, (uintptr_t)"\xB0\xF5\x3B\x6F", 4); // CMP.W R0, #0xBB0

    // Entry pool
    ARMHook::writeMemory(g_GTASAAdr + 0x40C99A, (uintptr_t)"\x4F\xF4\xC8\x30", 4); // mov.w r0, #0x19000
    ARMHook::writeMemory(g_GTASAAdr + 0x40C9A4, (uintptr_t)"\x4F\xF4\xA0\x50", 4); // mov.w r0, #0x1400
    ARMHook::writeMemory(g_GTASAAdr + 0x40C9A8, (uintptr_t)"\x4F\xF4\xA0\x55", 4); // mov.w r5, #0x1400
    ARMHook::writeMemory(g_GTASAAdr + 0x40C9EC, (uintptr_t)"\xB0\xF5\xA0\x5F", 4); // cmp.w r0, #0x1400

    // Ped pool
    ARMHook::writeMemory(g_GTASAAdr + 0x40CA04, (uintptr_t)"\x45\xF6\xC8\x60\xC0\xF2\x06\x00", 8); // movw r0, #0x5ec8 | movt r0, #6
    ARMHook::writeMemory(g_GTASAAdr + 0x40CA12, (uintptr_t)"\xD2\x20", 2); // MOVS R0, #0xD2
    ARMHook::writeMemory(g_GTASAAdr + 0x40CA14, (uintptr_t)"\xD2\x25", 2); // MOVS R5, #0xD2
    ARMHook::writeMemory(g_GTASAAdr + 0x40CA56, (uintptr_t)"\xD2\x28", 2); // CMP  R0, #0xD2

    // CPedIntelligence pool
    // ARMHook::writeMemory(g_GTASAAdr + 0x40CEBC, (uintptr_t)"\x42\xF2\xB0\x00\xC0\xF2\x02\x00\x00\x46", 10); // movw r0, #0x20b0 | movt r0, #2 | mov r0, r0
    // ARMHook::writeMemory(g_GTASAAdr + 0x40CECA, (uintptr_t)"\xD2\x20", 2); // MOVS R0, #0xD2
    // ARMHook::writeMemory(g_GTASAAdr + 0x40CECC, (uintptr_t)"\xD2\x25", 2); // MOVS R5, #0xD2
    // ARMHook::writeMemory(g_GTASAAdr + 0x40CF08, (uintptr_t)"\xD2\x28", 2); // CMP  R0, 0xD2

    // Vehicle pool
    // ARMHook::writeMemory(g_GTASAAdr + 0x40CA6C, (uintptr_t)"\x47\xF2\xC0\x70\xC0\xF2\x4F\x00", 8); // movw r0, #0x77c0 | movt r0, #0x4f
    // ARMHook::writeMemory(g_GTASAAdr + 0x40CA7A, (uintptr_t)"\x5F\xF4\xFA\x60", 2); // MOVS R0, #0x7D0
    // ARMHook::writeMemory(g_GTASAAdr + 0x40CA7C, (uintptr_t)"\x5F\xF4\xFA\x65", 2); // MOVS R5, #0x7D0
    // ARMHook::writeMemory(g_GTASAAdr + 0x40CABE, (uintptr_t)"\xB0\xF5\xFA\x6F", 2); // CMP  R0, 0x7D0

    ApplyRadarPatches();
}

void ApplySCAndPatches()
{
	FLog("ApplySCAndPatches");

	// SocialClub Patch #1
    ARMHook::unprotect(g_SCANDAdr+0x31C149);
    *(bool*)(g_SCANDAdr+0x31C149) = true;
}

void ApplySAMPPatchesInGame()
{
	FLog("ApplySAMPPatchesInGame");

    // CTheZones::ZonesVisited[100]
	memset((void*)(g_GTASAAdr + 0x98D252), 1, 100);

	// CTheZones::ZonesRevealed
	*(uint32_t*)(g_GTASAAdr + 0x98D2B8) = 100;

    ARMHook::unprotect(g_GTASAAdr + 0x5E4978);
    ARMHook::unprotect(g_GTASAAdr + 0x5E4990);
    *(uint8_t *)(g_GTASAAdr + 0x5E4978) = g_fps;
    *(uint8_t *)(g_GTASAAdr + 0x5E4990) = g_fps;

    // Draw distance hack (LAGS?)
    /*ARMHook::unprotect(g_GTASAAdr+0x41F300);
    *(uint32_t *)(g_GTASAAdr+0x41F300) = 0x41C80000; // 0x41C80000;
    ARMHook::unprotect(g_GTASAAdr+0x41FF6C);
    *(uint32_t *)(g_GTASAAdr+0x41FF6C) = 0x40A00000; // 0x40A00000;*/
}
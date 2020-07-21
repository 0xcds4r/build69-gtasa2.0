#include "main.h"
#include "game.h"
#include "net/netgame.h"
#include "util/armhook.h"
#include "chatwindow.h"

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CChatWindow* pChatWindow;

PAD_KEYS LocalPlayerKeys;
PAD_KEYS RemotePlayerKeys[PLAYER_PED_SLOTS];

uintptr_t dwCurPlayerActor = 0;
uint8_t byteCurPlayer = 0;
uint8_t byteCurDriver = 0;

uint16_t (*CPad__GetPedWalkLeftRight)(uintptr_t thiz);
uint16_t CPad__GetPedWalkLeftRight_Hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		// Remote player
		uint16_t dwResult = RemotePlayerKeys[byteCurPlayer].wKeyLR;
		if((dwResult == 0xFF80 || dwResult == 0x80) && RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_WALK]) {
			dwResult = 0x40;
		}

		return dwResult;
	}
	else
	{
		// Local player
		LocalPlayerKeys.wKeyLR = CPad__GetPedWalkLeftRight(thiz);
		return LocalPlayerKeys.wKeyLR;
	}
}

uint16_t (*CPad__GetPedWalkUpDown)(uintptr_t thiz);
uint16_t CPad__GetPedWalkUpDown_Hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		// Remote player
		uint16_t dwResult = RemotePlayerKeys[byteCurPlayer].wKeyUD;
		if((dwResult == 0xFF80 || dwResult == 0x80) && RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_WALK]) {
			dwResult = 0x40;
		}

		return dwResult;
	}
	else
	{
		// Local player
		LocalPlayerKeys.wKeyUD = CPad__GetPedWalkUpDown(thiz);
		return LocalPlayerKeys.wKeyUD;
	}
}

uint32_t (*CPad__GetSprint)(uintptr_t thiz, uint32_t unk);
uint32_t CPad__GetSprint_Hook(uintptr_t thiz, uint32_t unk)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SPRINT];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT] = CPad__GetSprint(thiz, unk);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT];
	}
}

uint32_t (*CPad__JumpJustDown)(uintptr_t thiz);
uint32_t CPad__JumpJustDown_Hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		if(!RemotePlayerKeys[byteCurPlayer].bIgnoreJump && RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP] && !RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE])
		{
			RemotePlayerKeys[byteCurPlayer].bIgnoreJump = true;
			return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
		}

		return 0;
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__JumpJustDown(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t (*CPad__GetJump)(uintptr_t thiz);
uint32_t CPad__GetJump_Hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		if(RemotePlayerKeys[byteCurPlayer].bIgnoreJump) return 0;
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__JumpJustDown(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t (*CPad__GetAutoClimb)(uintptr_t thiz);
uint32_t CPad__GetAutoClimb_Hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__GetAutoClimb(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t (*CPad__GetAbortClimb)(uintptr_t thiz);
uint32_t CPad__GetAbortClimb_Hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SECONDARY_ATTACK];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = CPad__GetAutoClimb(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK];
	}
}

uint32_t (*CPad__DiveJustDown)();
uint32_t CPad__DiveJustDown_Hook()
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		// remote player
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = CPad__DiveJustDown();
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE];
	}
}

uint32_t (*CPad__SwimJumpJustDown)(uintptr_t thiz);
uint32_t CPad__SwimJumpJustDown_Hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP];
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = CPad__SwimJumpJustDown(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP];
	}
}

uint32_t (*CPad__DuckJustDown)(uintptr_t thiz, int unk);
uint32_t CPad__DuckJustDown_Hook(uintptr_t thiz, int unk)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		uint32_t dwResult = CPad__DuckJustDown(thiz, unk);
		if(dwResult) {
			return 1;
		}

		return 0;
	}
	else
	{
		LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH] = CPad__DuckJustDown(thiz, unk);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH];
	}
}

uint32_t (*CPad__MeleeAttackJustDown)(uintptr_t thiz);
uint32_t CPad__MeleeAttackJustDown_Hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		if( RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE] &&
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_SECONDARY_ATTACK]) 
		{
			return 2;
		}
			
		return RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_FIRE];
	}
	else
	{
		uint32_t dwResult = CPad__MeleeAttackJustDown(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = dwResult;

		return dwResult;
	}
}

uint32_t (*CPad__GetBlock)(uintptr_t thiz);
uint32_t CPad__GetBlock_Hook(uintptr_t thiz)
{
	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		if( RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_JUMP] &&
			RemotePlayerKeys[byteCurPlayer].bKeys[ePadKeys::KEY_HANDBRAKE])
			return 1;

		return 0;
	}
	else
	{
		return CPad__GetBlock(thiz);
	}
}

int16_t (*CPad__GetSteeringLeftRight)(uintptr_t thiz);
int16_t CPad__GetSteeringLeftRight_Hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return (int16_t)RemotePlayerKeys[byteCurDriver].wKeyLR;
	}
	else
	{
		// local player
		LocalPlayerKeys.wKeyLR = CPad__GetSteeringLeftRight(thiz);
		return LocalPlayerKeys.wKeyLR;
	}
}

uint16_t (*CPad__GetSteeringUpDown)(uintptr_t thiz);
uint16_t CPad__GetSteeringUpDown_Hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].wKeyUD;
	}
	else
	{
		// local player
		LocalPlayerKeys.wKeyUD = CPad__GetSteeringUpDown(thiz);
		return LocalPlayerKeys.wKeyUD;
	}
}

uint16_t (*CPad__GetAccelerate)(uintptr_t thiz);
uint16_t CPad__GetAccelerate_Hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_SPRINT] ? 0xFF : 0x00;
	}
	else
	{
		// local player
		uint16_t wAccelerate = CPad__GetAccelerate(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT] = wAccelerate;
		return wAccelerate;
	}
}

uint16_t (*CPad__GetBrake)(uintptr_t thiz);
uint16_t CPad__GetBrake_Hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_JUMP] ? 0xFF : 0x00;
	}
	else
	{
		// local player
		uint16_t wBrake = CPad__GetBrake(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP] = wBrake;
		return wBrake;
	}
}

uint32_t (*CPad__GetHandBrake)(uintptr_t thiz);
uint32_t CPad__GetHandBrake_Hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_HANDBRAKE] ? 0xFF : 0x00;
	}
	else
	{
		// local player
		uint32_t handBrake = CPad__GetHandBrake(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE] = handBrake;
		return handBrake;
	}
}

uint32_t (*CPad__GetHorn)(uintptr_t thiz);
uint32_t CPad__GetHorn_Hook(uintptr_t thiz)
{
	if(byteCurDriver != 0)
	{
		// remote player
		return RemotePlayerKeys[byteCurDriver].bKeys[ePadKeys::KEY_CROUCH];
	}
	else
	{
		// local player
		uint32_t horn = CPad__GetHorn(thiz);
		LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH] = CPad__GetHorn(thiz);
		return LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH];
	}
}

void (*CPed__ProcessControl)(uintptr_t thiz);
void CPed__ProcessControl_Hook(uintptr_t thiz)
{
	// FLog("CPed__ProcessControl_Hook");

	dwCurPlayerActor = thiz;
	byteCurPlayer = FindPlayerNumFromPedPtr(dwCurPlayerActor);

	if(dwCurPlayerActor && (byteCurPlayer != 0))
	{
		ARMHook::makeNOP(g_GTASAAdr + 0x4A2A22, 2); 
    	CPed__ProcessControl(thiz);
    	ARMHook::writeMemory(g_GTASAAdr + 0x4A2A22, (uintptr_t)"\xF0\xF4\x42\xEB", 4); // +
	}
	else
	{
		ARMHook::writeMemory(g_GTASAAdr + 0x539BA6, (uintptr_t)"\xC4\xF8\x60\x55", 4); // +
    	(*CPed__ProcessControl)(thiz);
    	ARMHook::makeNOP(g_GTASAAdr + 0x539BA6, 2);
	}

	return;
}

void ProcessControl(VEHICLE_TYPE* pVehicle, uintptr_t call_addr)
{
	uintptr_t pVehiclePtr = (uintptr_t)pVehicle;

	if(!pVehiclePtr) {
		return;
	}

	uintptr_t pDriver = *(uintptr_t *)(pVehiclePtr + 0x464);
	uintptr_t pTemp = *(uintptr_t *)(pVehiclePtr + 1124); 

	// FLog("ProcessControl pDriver = 0x%x | pTemp = 0x%x", pDriver, pTemp);

	if(pVehicle && pDriver) {
		byteCurDriver = FindPlayerNumFromPedPtr(pDriver);
	}

	if( pDriver && *(uint32_t *)(pTemp + 1436) == 0 &&
		pDriver != (uintptr_t)GamePool_FindPlayerPed() && 
		*(uint8_t*)(g_GTASAAdr+0x96B9C4) == 0) // 96B9C4 ; CWorld::PlayerInFocus
	{
		*(uint8_t*)(g_GTASAAdr+0x96B9C4) = 0;

		*(uint32_t *)(pTemp + 1436) = 4;

		// CAEVehicleAudioEntity::Service
		(( void (*)(uintptr_t))(g_GTASAAdr+0x3ACDB5))(pVehiclePtr+0x13C); // 0x3ACDB5 thiz + 0x13C

		*(uint32_t *)(pTemp + 1436) = 0;
	} 
	else
	{
		// CAEVehicleAudioEntity::Service
		(( void (*)(uintptr_t))(g_GTASAAdr+0x3ACDB5))(pVehiclePtr+0x13C); // 0x3ACDB5 thiz + 0x13C
	}

	/* -------------------------- adjust address -------------------------- */
	// protection or dolboebizm? YES!

	// CAutomobile
    if(call_addr == 0x553DD5)
    {
    	(( void (*)(VEHICLE_TYPE*))(g_GTASAAdr+0x553DD4+1))(pVehicle);
    }

    // CBoat
    if(call_addr == 0x56BE51)
    {
    	(( void (*)(VEHICLE_TYPE*))(g_GTASAAdr+0x56BE50+1))(pVehicle);
    }

    // CBike
    if(call_addr == 0x561A21)
    {
    	(( void (*)(VEHICLE_TYPE*))(g_GTASAAdr+0x561A20+1))(pVehicle);
    }

    // CPlane
    if(call_addr == 0x575C88)
    {
    	(( void (*)(VEHICLE_TYPE*))(g_GTASAAdr+0x575C88+1))(pVehicle);
    }

    // CHeli
    if(call_addr == 0x571238)
    {
    	(( void (*)(VEHICLE_TYPE*))(g_GTASAAdr+0x571238+1))(pVehicle);
    }

    // CBmx
    if(call_addr == 0x568B15)
    {
    	(( void (*)(VEHICLE_TYPE*))(g_GTASAAdr+0x568B14+1))(pVehicle);
    }

    // CMonsterTruck
    if(call_addr == 0x5747F4)
    {
    	(( void (*)(VEHICLE_TYPE*))(g_GTASAAdr+0x5747F4+1))(pVehicle);
    }

    // CQuadBike
    if(call_addr == 0x57A280)
    {
    	(( void (*)(VEHICLE_TYPE*))(g_GTASAAdr+0x57A280+1))(pVehicle);
    }

    // CTrain
    if(call_addr == 0x57D030)
    {
    	(( void (*)(VEHICLE_TYPE*))(g_GTASAAdr+0x57D030+1))(pVehicle);
    }
}

void AllVehicles__ProcessControl_Hook(uintptr_t thiz)
{
	VEHICLE_TYPE *pVehicle = (VEHICLE_TYPE*)thiz;
	uintptr_t this_vtable = pVehicle->entity.vtable;
	this_vtable -= g_GTASAAdr;

	uintptr_t call_addr = 0;

	// FLog("AllVehicles__ProcessControl_Hook: this_vtable = 0x%x", this_vtable);

	switch(this_vtable)
	{
		// CAutomobile
		case 0x66D688:
		call_addr = 0x553DD5; // +
		break;

		// CBoat
		case 0x66DA30:
		call_addr = 0x56BE51; // +
		break;

		// CBike
		case 0x66D800:
		call_addr = 0x561A21; // +
		break;

		// CPlane
		case 0x66DD94: 
		call_addr = 0x575C88; // +
		break;

		// CHeli
		case 0x66DB44: 
		call_addr = 0x571238; // +
		break;

		// CBmx
		case 0x66D918: // ?
		call_addr = 0x568B15; // +
		break;

		// CMonsterTruck
		case 0x66DC6C: 
		call_addr = 0x5747F4; // +
		break;

		// CQuadBike
		case 0x66DEBC:
		call_addr = 0x57A280; // +
		break;

		// CTrain
		case 0x66E10C:
		call_addr = 0x57D030; // +
		break;
	}

	ProcessControl(pVehicle, call_addr);
}

void HookCPad()
{
	memset(&LocalPlayerKeys, 0, sizeof(PAD_KEYS));

	SetupGameHook(g_GTASAAdr + 0x4C4778, (uintptr_t)CPed__ProcessControl_Hook, (uintptr_t*)&CPed__ProcessControl);

	ARMHook::installMethodHook(g_GTASAAdr + 0x66D6B4, (uintptr_t)AllVehicles__ProcessControl_Hook);
 	ARMHook::installMethodHook(g_GTASAAdr + 0x66DA5C, (uintptr_t)AllVehicles__ProcessControl_Hook);
 	ARMHook::installMethodHook(g_GTASAAdr + 0x66D82C, (uintptr_t)AllVehicles__ProcessControl_Hook);
 	ARMHook::installMethodHook(g_GTASAAdr + 0x66DDC0, (uintptr_t)AllVehicles__ProcessControl_Hook);
 	ARMHook::installMethodHook(g_GTASAAdr + 0x66DB70, (uintptr_t)AllVehicles__ProcessControl_Hook);
 	ARMHook::installMethodHook(g_GTASAAdr + 0x66D944, (uintptr_t)AllVehicles__ProcessControl_Hook);
 	ARMHook::installMethodHook(g_GTASAAdr + 0x66DC98, (uintptr_t)AllVehicles__ProcessControl_Hook);
 	ARMHook::installMethodHook(g_GTASAAdr + 0x66DEE8, (uintptr_t)AllVehicles__ProcessControl_Hook);
 	ARMHook::installMethodHook(g_GTASAAdr + 0x66E138, (uintptr_t)AllVehicles__ProcessControl_Hook);

 	SetupGameHook(g_GTASAAdr + 0x3FA248, (uintptr_t)CPad__GetPedWalkUpDown_Hook, (uintptr_t*)&CPad__GetPedWalkUpDown);
 	SetupGameHook(g_GTASAAdr + 0x3FA1C8, (uintptr_t)CPad__GetPedWalkLeftRight_Hook, (uintptr_t*)&CPad__GetPedWalkLeftRight);
 	SetupGameHook(g_GTASAAdr + 0x3FBD60, (uintptr_t)CPad__GetSprint_Hook, (uintptr_t*)&CPad__GetSprint);                                                            
 	SetupGameHook(g_GTASAAdr + 0x3FBC5C, (uintptr_t)CPad__JumpJustDown_Hook, (uintptr_t*)&CPad__JumpJustDown);                                                         
 	SetupGameHook(g_GTASAAdr + 0x3FBC08, (uintptr_t)CPad__GetJump_Hook, (uintptr_t*)&CPad__GetJump);                                                              
 	SetupGameHook(g_GTASAAdr + 0x3FBABC, (uintptr_t)CPad__GetAutoClimb_Hook, (uintptr_t*)&CPad__GetAutoClimb);                                                         
 	SetupGameHook(g_GTASAAdr + 0x3FBCC0, (uintptr_t)CPad__DiveJustDown_Hook, (uintptr_t*)&CPad__DiveJustDown);                                                         
 	SetupGameHook(g_GTASAAdr + 0x3FBD08, (uintptr_t)CPad__SwimJumpJustDown_Hook, (uintptr_t*)&CPad__SwimJumpJustDown);                                                     
 	SetupGameHook(g_GTASAAdr + 0x3FBA4C, (uintptr_t)CPad__DuckJustDown_Hook, (uintptr_t*)&CPad__DuckJustDown);                                                         
 	SetupGameHook(g_GTASAAdr + 0x3FAFBC, (uintptr_t)CPad__MeleeAttackJustDown_Hook, (uintptr_t*)&CPad__MeleeAttackJustDown);                                                  
 	SetupGameHook(g_GTASAAdr + 0x3FB2D0, (uintptr_t)CPad__GetBlock_Hook, (uintptr_t*)&CPad__GetBlock);                                                             
 	SetupGameHook(g_GTASAAdr + 0x3F9B04, (uintptr_t)CPad__GetSteeringLeftRight_Hook, (uintptr_t*)&CPad__GetSteeringLeftRight);                                                 
 	SetupGameHook(g_GTASAAdr + 0x3F9CD4, (uintptr_t)CPad__GetSteeringUpDown_Hook, (uintptr_t*)&CPad__GetSteeringUpDown);                                                    
 	SetupGameHook(g_GTASAAdr + 0x3FB300, (uintptr_t)CPad__GetAccelerate_Hook, (uintptr_t*)&CPad__GetAccelerate);                                                        
 	SetupGameHook(g_GTASAAdr + 0x3FA95C, (uintptr_t)CPad__GetBrake_Hook, (uintptr_t*)&CPad__GetBrake);                                                             
 	SetupGameHook(g_GTASAAdr + 0x3FA790, (uintptr_t)CPad__GetHandBrake_Hook, (uintptr_t*)&CPad__GetHandBrake);                                                         
 	SetupGameHook(g_GTASAAdr + 0x3FA5C8, (uintptr_t)CPad__GetHorn_Hook, (uintptr_t*)&CPad__GetHorn);                                                          
}
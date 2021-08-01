#include "../main.h"
#include "game.h"
#include "net/netgame.h"
#include "util/armhook.h"

extern CGame* pGame;
extern CNetGame *pNetGame;

CPlayerPed::CPlayerPed()
{
	m_dwGTAId = 1;
	m_pPed = (PED_TYPE*)GamePool_FindPlayerPed();
	m_pEntity = (ENTITY_TYPE*)GamePool_FindPlayerPed();

	m_bytePlayerNumber = 0;
	SetPlayerPedPtrRecord(m_bytePlayerNumber,(uintptr_t)m_pPed);
	ScriptCommand(&set_actor_weapon_droppable, m_dwGTAId, 1);
	ScriptCommand(&set_actor_can_be_decapitated, m_dwGTAId, 0);

	m_dwArrow = 0;

	ResetAttachedObjects();
}

CPlayerPed::CPlayerPed(uint8_t bytePlayerNumber, int iSkin, float fX, float fY, float fZ, float fRotation)
{
	uint32_t dwPlayerActorID = 0;
	int iPlayerNum = bytePlayerNumber;

	m_pPed = nullptr;
	m_dwGTAId = 0;
	m_dwArrow = 0;

	ScriptCommand(&create_player, &iPlayerNum, fX, fY, fZ, &dwPlayerActorID);
	ScriptCommand(&create_actor_from_player, &iPlayerNum, &dwPlayerActorID);

	m_dwGTAId = dwPlayerActorID;
	m_pPed = GamePool_Ped_GetAt(m_dwGTAId);
	m_pEntity = (ENTITY_TYPE*)GamePool_Ped_GetAt(m_dwGTAId);

	m_bytePlayerNumber = bytePlayerNumber;
	SetPlayerPedPtrRecord(m_bytePlayerNumber, (uintptr_t)m_pPed);
	ScriptCommand(&set_actor_weapon_droppable, m_dwGTAId, 1);
	ScriptCommand(&set_actor_immunities, m_dwGTAId, 0, 0, 1, 0, 0);
	ScriptCommand(&set_actor_can_be_decapitated, m_dwGTAId, 0);

	if(pNetGame) {
		SetMoney(pNetGame->m_iDeathDropMoney);
	}

	SetModelIndex(iSkin);
	ForceTargetRotation(fRotation);

	MATRIX4X4 mat;
	GetMatrix(&mat);
	mat.pos.X = fX;
	mat.pos.Y = fY;
	mat.pos.Z = fZ + 0.15f;
	SetMatrix(mat);

	memset(&RemotePlayerKeys[m_bytePlayerNumber], 0, sizeof(PAD_KEYS));

	ResetAttachedObjects();
}

CPlayerPed::~CPlayerPed()
{
	Destroy();
}

void CPlayerPed::ResetAttachedObjects()
{
	for(int i = 0; i < 10; i++)
	{
		m_bObjectSlotUsed[i] = false;
		m_pAttachedObjects[i] = nullptr;
	}
}

void CPlayerPed::Destroy()
{
	memset(&RemotePlayerKeys[m_bytePlayerNumber], 0, sizeof(PAD_KEYS));
	SetPlayerPedPtrRecord(m_bytePlayerNumber, 0);

	if(!m_pPed || !GamePool_Ped_GetAt(m_dwGTAId) || m_pPed->entity.vtable == 0x667D24)
	{
		FLog("CPlayerPed::Destroy: invalid pointer/vtable");
		m_pPed = nullptr;
		m_pEntity = nullptr;
		m_dwGTAId = 0;
		return;
	}

	if(IsHaveAttachedObject()) {
		RemoveAllAttachedObjects();
	}

	FLog("Removing from vehicle..");
	if(IsInVehicle()) {
		RemoveFromVehicleAndPutAt(100.0f, 100.0f, 10.0f);
	}

	FLog("Setting flag state..");
	uintptr_t dwPedPtr = (uintptr_t)m_pPed;
	*(uint32_t*)(*(uintptr_t*)(dwPedPtr + 1088) + 76) = 0;

	// CPlayerPed::Destructor
	FLog("Calling destructor..");
	(( void (*)(PED_TYPE*))(*(void**)(m_pPed->entity.vtable+0x4)))(m_pPed);

	m_pPed = nullptr;
	m_pEntity = nullptr;
}

// 0.3.7
bool CPlayerPed::IsInVehicle()
{
	if(!m_pPed) return false;

	if((*(uint8_t *)((uintptr_t)m_pPed + 0x485) & 1) == 1) {
		return true;
	}

	return false;
}

// 0.3.7
bool CPlayerPed::IsAPassenger()
{
	if(m_pPed)
	{
		uintptr_t pVehicle = *(uintptr_t *)((uintptr_t)m_pPed + 1424);

		if(pVehicle)
		{
			if(*(uint8_t *)((uintptr_t)m_pPed + 1157) << 31 != 0) 
			{
				if ( *(uintptr_t *)((uintptr_t)pVehicle + 1124) != (uintptr_t)m_pPed ) {
    				return true;
				}

				uint16_t v5 = (uint16_t)(*(uint16_t *)((uintptr_t)pVehicle + 38) - 569);
  				if ( v5 < 2 ) {
    				return true;
  				}
			}
		}
	}

	return false;
}

// 0.3.7
VEHICLE_TYPE* CPlayerPed::GetGtaVehicle()
{
	if(!m_pPed) {
		return nullptr;
	}

	uintptr_t pVehicle = *(uintptr_t *)((uintptr_t)m_pPed + 1424);
	return (VEHICLE_TYPE*)pVehicle;
}

// 0.3.7
void CPlayerPed::RemoveFromVehicleAndPutAt(float fX, float fY, float fZ)
{
	if(!GamePool_Ped_GetAt(m_dwGTAId)) {
		return;
	}

	if(m_pPed && *(VEHICLE_TYPE**)(m_pPed + 0x590)) {
		ScriptCommand(&remove_actor_from_car_and_put_at, m_dwGTAId, fX, fY, fZ);
	}
}

// 0.3.7 (2.0)
void CPlayerPed::SetInitialState()
{
	(( int (*)(uintptr_t, bool))(g_GTASAAdr+0x4C3745))((uintptr_t)m_pPed, false);
}

// 0.3.7
void CPlayerPed::SetHealth(float fHealth)
{
	if(!m_pPed) {
		return;
	}

	*(float*)((uintptr_t)m_pPed + 0x544) = fHealth;
}

// 0.3.7
float CPlayerPed::GetHealth()
{
	if(!m_pPed) {
		return 0.0f;
	}

	return *(float*)((uintptr_t)m_pPed + 0x544);
}

// 0.3.7
void CPlayerPed::SetArmour(float fArmour)
{
	if(!m_pPed) {
		return;
	}

	*(float*)((uintptr_t)m_pPed + 0x54C) = fArmour;
}

float CPlayerPed::GetArmour()
{
	if(!m_pPed) {
		return 0.0f;
	}

	return *(float*)((uintptr_t)m_pPed + 0x54C);
}

void CPlayerPed::SetInterior(uint8_t byteID)
{
	if(!m_pPed) {
		return;
	}

	ScriptCommand(&select_interior, byteID);
	ScriptCommand(&link_actor_to_interior, m_dwGTAId, byteID);

	MATRIX4X4 mat;
	GetMatrix(&mat);
	ScriptCommand(&refresh_streaming_at, mat.pos.X, mat.pos.Y);
}

void CPlayerPed::PutDirectlyInVehicle(int iVehicleID, int iSeat)
{
	if(!m_pPed) {
		return;
	}

	if(!GamePool_Vehicle_GetAt(iVehicleID)) {
		return;
	}

	if(!GamePool_Ped_GetAt(m_dwGTAId)) {
		return;
	}

	VEHICLE_TYPE *pVehicle = GamePool_Vehicle_GetAt(iVehicleID);
	if(!pVehicle) {
		return;
	}

	if(*(float*)((uintptr_t)pVehicle + 0x4CC) == 0.0f) {
		return;
	}

	if (pVehicle->entity.vtable == g_GTASAAdr+0x667D24) {
		return;
	}

	if(iSeat == 0)
	{
		if(*(uintptr_t**)((uintptr_t)pVehicle + 0x464) && *(VEHICLE_TYPE**)(m_pPed + 0x590)) return;
		ScriptCommand(&put_actor_in_car, m_dwGTAId, iVehicleID);
	}
	else
	{
		iSeat--;
		ScriptCommand(&put_actor_in_car2, m_dwGTAId, iVehicleID, iSeat);
	}

	if(m_pPed == GamePool_FindPlayerPed() && *(VEHICLE_TYPE**)(m_pPed + 0x590)) {
		pGame->GetCamera()->SetBehindPlayer();
	}

	if(pNetGame)
	{
		// nothing
	}
}

void CPlayerPed::EnterVehicle(int iVehicleID, bool bPassenger)
{
	if(!m_pPed) {
		return;
	}

	VEHICLE_TYPE* ThisVehicleType = nullptr;
	if((ThisVehicleType = GamePool_Vehicle_GetAt(iVehicleID)) == 0) {
		return;
	}

	if(!GamePool_Ped_GetAt(m_dwGTAId)) {
		return;
	}

	if(bPassenger)
	{
		ENTITY_TYPE* g_pEntity = (ENTITY_TYPE*)&ThisVehicleType->entity;
		if(*(uint16_t*)((uintptr_t)g_pEntity + 0x26) == TRAIN_PASSENGER &&
			(m_pPed == GamePool_FindPlayerPed()))
		{
			ScriptCommand(&put_actor_in_car2, m_dwGTAId, iVehicleID, -1);
		}
		else
		{
			ScriptCommand(&send_actor_to_car_passenger,m_dwGTAId,iVehicleID, 3000, -1);
		}
	}
	else
		ScriptCommand(&send_actor_to_car_driverseat, m_dwGTAId, iVehicleID, 3000);
}

// 0.3.7
void CPlayerPed::ExitCurrentVehicle()
{
	if(!m_pPed) return;
	if(!GamePool_Ped_GetAt(m_dwGTAId)) return;

	VEHICLE_TYPE* ThisVehicleType = 0;

	if(*(VEHICLE_TYPE**)(m_pPed + 0x590))
	{
		if(GamePool_Vehicle_GetIndex(*(VEHICLE_TYPE**)((uintptr_t)m_pPed + 0x590)))
		{
			int index = GamePool_Vehicle_GetIndex(*(VEHICLE_TYPE**)((uintptr_t)m_pPed + 0x590));
			ThisVehicleType = GamePool_Vehicle_GetAt(index);
			if(ThisVehicleType)
			{
				ENTITY_TYPE* g_pEntity = (ENTITY_TYPE*)&ThisVehicleType->entity;
				if(	*(uint16_t*)((uintptr_t)g_pEntity + 0x26) != TRAIN_PASSENGER &&
					*(uint16_t*)((uintptr_t)g_pEntity + 0x26) != TRAIN_PASSENGER_LOCO)
				{
					ScriptCommand(&make_actor_leave_car, m_dwGTAId, GetCurrentVehicleID());
				}
			}
		}
	}
}

// 0.3.7
int CPlayerPed::GetCurrentVehicleID()
{
	if(!m_pPed) {
		return 0;
	}

	VEHICLE_TYPE *pVehicle = *(VEHICLE_TYPE**)((uintptr_t)m_pPed + 0x590);
	return GamePool_Vehicle_GetIndex(pVehicle);
}

int CPlayerPed::GetVehicleSeatID()
{
	VEHICLE_TYPE *pVehicle = nullptr;

	if( GetActionTrigger() == ACTION_INCAR && (pVehicle = *(VEHICLE_TYPE**)((uintptr_t)m_pPed + 0x590)) != 0 ) 
	{
		if(pVehicle->pDriver == m_pPed) return 0;
		if(pVehicle->pPassengers[0] == m_pPed) return 1;
		if(pVehicle->pPassengers[1] == m_pPed) return 2;
		if(pVehicle->pPassengers[2] == m_pPed) return 3;
		if(pVehicle->pPassengers[3] == m_pPed) return 4;
		if(pVehicle->pPassengers[4] == m_pPed) return 5;
		if(pVehicle->pPassengers[5] == m_pPed) return 6;
		if(pVehicle->pPassengers[6] == m_pPed) return 7;
	}

	return (-1);
}

// 0.3.7
void CPlayerPed::TogglePlayerControllable(bool bToggle)
{
	MATRIX4X4 mat;

	if(!GamePool_Ped_GetAt(m_dwGTAId)) return;

	if(!bToggle)
	{
		SetControllable(0);
		ScriptCommand(&lock_actor, m_dwGTAId, 1);
	}
	else
	{
		SetControllable(1);
		ScriptCommand(&lock_actor, m_dwGTAId, 0);
		if(!IsInVehicle()) 
		{
			GetMatrix(&mat);
			TeleportTo(mat.pos.X, mat.pos.Y, mat.pos.Z);
		}
	}
}

bool bControllable = true;
void CPlayerPed::SetControllable(bool bToggle)
{
	if(!m_pPed) return;
	if(!GamePool_Ped_GetAt(m_dwGTAId)) return;

	if(!bToggle && bControllable)
	{
		ScriptCommand(&toggle_player_controllable, m_bytePlayerNumber, 0);
		bControllable = false;
	}
	else if(bToggle && !bControllable)
	{
		ScriptCommand(&toggle_player_controllable, m_bytePlayerNumber, 1);
		bControllable = true;
	}
}

// 0.3.7
void CPlayerPed::SetModelIndex(unsigned int uiModel)
{
	if(!GamePool_Ped_GetAt(m_dwGTAId)) {
		return;
	}

	if(!IsPedModel(uiModel)) {
		uiModel = 0;
	}

	if(m_pPed)
	{
		// CClothes::RebuildPlayer nulled
		ARMHook::makeRET(g_GTASAAdr+0x45751C);

		DestroyFollowPedTask();
		CEntity::SetModelIndex(uiModel);

		// reset the Ped Audio Attributes
		(( void (*)(uintptr_t, uintptr_t))(g_GTASAAdr+0x39CE69))(((uintptr_t)m_pPed+0x298), (uintptr_t)m_pPed);
	}
}

void CPlayerPed::DestroyFollowPedTask()
{
	// nothing
}

void CPlayerPed::GiveWeapon(int iWeaponID, int iAmmo)
{
	// nothing
}

void CPlayerPed::SetArmedWeapon(int iWeaponID)
{
	// nothing
}

uint8_t CPlayerPed::GetCurrentWeapon()
{
	return 0;
	// nothing
}

void CPlayerPed::ClearAllWeapons()
{
	// nothing
}

void CPlayerPed::ResetDamageEntity()
{
	// nothing
}

// 0.3.7
void CPlayerPed::RestartIfWastedAt(VECTOR *vecRestart, float fRotation)
{	
	ScriptCommand(&restart_if_wasted_at, vecRestart->X, vecRestart->Y, vecRestart->Z, fRotation, 0);
}

// 0.3.7
void CPlayerPed::ForceTargetRotation(float fRotation)
{
	if(!m_pPed) {
		return;
	}

	if(!GamePool_Ped_GetAt(m_dwGTAId)) {
		return;
	}

	*(float*)((uintptr_t)m_pPed + 0x55C) = DegToRad(fRotation);
	*(float*)((uintptr_t)m_pPed + 0x560) = DegToRad(fRotation);

	ScriptCommand(&set_actor_z_angle, m_dwGTAId, fRotation);
}

void CPlayerPed::SetRotation(float fRotation)
{
	if(!m_pPed) {
		return;
	}

	if(!GamePool_Ped_GetAt(m_dwGTAId)) {
		return;
	}

	*(float*)((uintptr_t)m_pPed + 0x55C) = DegToRad(fRotation);
	*(float*)((uintptr_t)m_pPed + 0x560) = DegToRad(fRotation);
}

// 0.3.7
uint8_t CPlayerPed::GetActionTrigger()
{
	if(!m_pPed) {
		return 0;
	}

	return *(uint8_t*)((uintptr_t)m_pPed + 0x44C);
}

// 0.3.7
bool CPlayerPed::IsDead()
{
	if(!m_pPed) {
		return true;
	}

	if(*(float*)((uintptr_t)m_pPed + 0x544) > 0.0f) {
		return false;
	}

	return true;
}

void CPlayerPed::SetMoney(int iAmount)
{
	if(!GamePool_Ped_GetAt(m_dwGTAId)) {
		return;
	}

	ScriptCommand(&set_actor_money, m_dwGTAId, 0);
	ScriptCommand(&set_actor_money, m_dwGTAId, iAmount);
}

// 0.3.7
void CPlayerPed::ShowMarker(uint32_t iMarkerColorID)
{
	if(m_dwArrow) {
		HideMarker();
	}

	ScriptCommand(&create_arrow_above_actor, m_dwGTAId, &m_dwArrow);
	ScriptCommand(&set_marker_color, m_dwArrow, iMarkerColorID);
	ScriptCommand(&show_on_radar2, m_dwArrow, 2);
}

// 0.3.7
void CPlayerPed::HideMarker()
{
	if(m_dwArrow) {
		ScriptCommand(&disable_marker, m_dwArrow);
	}

	m_dwArrow = 0;
}

// 0.3.7
void CPlayerPed::SetFightingStyle(int iStyle)
{
	if(!m_pPed) {
		return;
	}

	ScriptCommand(&set_fighting_style, m_dwGTAId, iStyle, 6);
}

// 0.3.7
void CPlayerPed::ApplyAnimation( char *szAnimName, char *szAnimFile, float fT,
								 int opt1, int opt2, int opt3, int opt4, int iUnk )
{
	int iWaitAnimLoad = 0;

	if(!m_pPed) {
		return;
	} 
		
	if(!GamePool_Ped_GetAt(m_dwGTAId)) {
		return;
	}

	if(!strcasecmp(szAnimFile,"SEX")) {
		return;
	}

	if(!pGame->IsAnimationLoaded(szAnimFile))
	{
		pGame->RequestAnimation(szAnimFile);
		while(!pGame->IsAnimationLoaded(szAnimFile))
		{
			usleep(1000);
			iWaitAnimLoad++;

			if(iWaitAnimLoad > 15) {
				return;
			}
		}
	}

	ScriptCommand(&apply_animation, m_dwGTAId, szAnimName, szAnimFile, fT, opt1, opt2, opt3, opt4, iUnk);
}

void CPlayerPed::FindDeathReasonAndResponsiblePlayer(PLAYERID *nPlayer)
{
	*nPlayer = INVALID_PLAYER_ID;
}

// 0.3.7
void CPlayerPed::GetBonePosition(int iBoneID, VECTOR* vecOut)
{
	if(!m_pPed) return;
	if(m_pEntity->vtable == g_GTASAAdr+0x667D24) return;

	(( void (*)(PED_TYPE*, VECTOR*, int, int))(g_GTASAAdr+0x4A4A9D))(m_pPed, vecOut, iBoneID, 0);
}

ENTITY_TYPE* CPlayerPed::GetEntityUnderPlayer()
{
	// nothing
	return 0;
}

void CPlayerPed::ClumpUpdateAnimations(float step, int flag)
{
	if(m_pPed)
	{
		uintptr_t pRwObj = GetRWObject();

		if(pRwObj)
		{
			((void (*)(uintptr_t, float, int))(g_GTASAAdr+0x38BF01))(pRwObj, step, flag);
		}
	}
}

// 0.3.7
void CPlayerPed::GetBonePosition(VECTOR &out, unsigned int boneid, bool update)
{
	if(!GamePool_Ped_GetAt(m_dwGTAId)) return;

	if(m_pPed)
	{
		// CPed::GetBonePosition(RwV3d &,uint,bool)	.text	004A4A9C	0000009C	00000030	00000000	R	.	.	.	B	T	.
		((void(*)(uintptr_t, VECTOR &, unsigned int, bool))(g_GTASAAdr+0x4A4A9C+1))((uintptr_t)m_pPed, out, boneid, update);
	}
}

bool CPlayerPed::IsHaveAttachedObject()
{
	for(int i=0; i<10; i++)
	{
		if(m_bObjectSlotUsed[i])
		{
			return true;
		}
	}
	return false;
}

void CPlayerPed::RemoveAllAttachedObjects()
{
	for(int index = 0; index < 10; index++)
	{
		if(GetObjectSlotState(index))
		{
			CObject *pObject = m_pAttachedObjects[index];
			if(pObject)
			{
				delete pObject;
				m_pAttachedObjects[index] = nullptr;
			}
			memset(&m_AttachedObjectInfo[index], 0, sizeof(ATTACHED_OBJECT));
			m_bObjectSlotUsed[index] = false;
		}
	}
}

bool CPlayerPed::GetBoneMatrix(MATRIX4X4 *matOut, int iBoneID)
{
	if(!m_pPed) return false;
	if(m_pPed->entity.vtable == g_GTASAAdr+0x667D24) return false;

	uintptr_t pdwRenderWare = GetRWObject();
	if(!pdwRenderWare) return false;

	// GetAnimHierarchyFromSkinClump(RpClump *)	.text	005D1020	0000001A	00000010	FFFFFFF8	R	.	.	.	.	.	.
	uintptr_t pAnimHierarchy = (( uintptr_t (*)(uintptr_t))(g_GTASAAdr+0x5D1020+1))(pdwRenderWare);

	// RpHAnimIDGetIndex(RpHAnimHierarchy *,int)	.text	001C2C90	00000024			R	.	.	.	.	.	.
	int iAnimIndex = (( uintptr_t (*)(uintptr_t, uintptr_t))(g_GTASAAdr+0x1C2C90+1))(pAnimHierarchy, iBoneID) << 6;

	MATRIX4X4 *mat = (MATRIX4X4*)(iAnimIndex + *(uintptr_t*)(pAnimHierarchy+8));

	if(isnan(mat->right.X) || isnan(mat->right.Y) || isnan(mat->right.Z) || isnan(mat->up.X) || isnan(mat->up.Y) || isnan(mat->up.Z)) return false;
	if(isnan(mat->at.X) || isnan(mat->at.Y) || isnan(mat->at.Z)) return false;

	if(std::isinf(mat->right.X) || std::isinf(mat->right.Y) || std::isinf(mat->right.Z) || std::isinf(mat->up.X) || std::isinf(mat->up.Y) || std::isinf(mat->up.Z)) return false;
	if(std::isinf(mat->at.X) || std::isinf(mat->at.Y) || std::isinf(mat->at.Z)) return false;

	if(mat->right.X > 20000.0 || mat->right.X < -20000.0 || mat->right.Y > 20000.0 || mat->right.Y < -20000.0 || mat->right.Z > 20000.0 || mat->right.Z < -20000.0) return false;
	if(mat->at.X > 20000.0 || mat->at.X < -20000.0 || mat->at.Y > 20000.0 || mat->at.Y < -20000.0 || mat->at.Z > 20000.0 || mat->at.Z < -20000.0) return false;
	if(mat->up.X > 20000.0 || mat->up.X < -20000.0 || mat->up.Y > 20000.0 || mat->up.Y < -20000.0 || mat->up.Z > 20000.0 || mat->up.Z < -20000.0) return false;

	if(mat)
	{
		memcpy(matOut, mat, sizeof(MATRIX4X4));

		return true;
	}

	return false;
}

void CPlayerPed::UpdateAttachedObject(bool create, uint32_t index, uint32_t model, uint32_t bone, VECTOR vecOffset, VECTOR vecRotation, VECTOR vecScale, uint32_t materialcolor1, uint32_t materialcolor2)
{
	return;

    if(!m_pPed) return;
    if(index < 0 || index >= 10)
        return;
 
    if(create)
    {
        if(m_pPed->entity.vtable == g_GTASAAdr+0x667D24) return;
        if(!GetRWObject()) return;
 
        if(bone > 0 && bone <= 18)
        {
            if(GetObjectSlotState(index))
            {
                CObject *pObject = m_pAttachedObjects[index];
                if(pObject)
                {
                    delete pObject;
                    m_pAttachedObjects[index] = nullptr;
                }
                memset(&m_AttachedObjectInfo[index], 0, sizeof(ATTACHED_OBJECT));
                m_bObjectSlotUsed[index] = false;
            }
            ATTACHED_OBJECT stAttachedObject;
            stAttachedObject.iModel = model;
            stAttachedObject.iBoneID = bone;
            stAttachedObject.vecOffset = vecOffset;
            stAttachedObject.vecRotation = vecRotation;
            stAttachedObject.vecScale = vecScale;
            stAttachedObject.dwMaterialColor1 = materialcolor1;
            stAttachedObject.dwMaterialColor2 = materialcolor2;
 
            MATRIX4X4 matrix;
            GetMatrix(&matrix);
            memcpy(&m_AttachedObjectInfo[index], &stAttachedObject, sizeof(ATTACHED_OBJECT));
            CObject *pNewObject = new CObject(model,
                matrix.pos.X,
                matrix.pos.Y,
                matrix.pos.Z,
                vecRotation, 200.0f);
            m_pAttachedObjects[index] = pNewObject;
            m_bObjectSlotUsed[index] = true;
        }
    }
    else
    {
        CObject *pObject = m_pAttachedObjects[index];
        if(pObject)
        {
            delete pObject;
            m_pAttachedObjects[index] = nullptr;
        }
 
        memset(&m_AttachedObjectInfo[index], 0, sizeof(ATTACHED_OBJECT));
        m_bObjectSlotUsed[index] = false;
    }
}
 
bool CPlayerPed::GetObjectSlotState(int iObjectIndex)
{
    if(iObjectIndex < 0 || iObjectIndex >= 10)
        return false;
 
    return m_bObjectSlotUsed[iObjectIndex];
}

void CPlayerPed::ProcessAttachedObjects()
{
	if(!m_pPed || !m_dwGTAId)
		return;

	bool bAnimUpdated = false;
	MATRIX4X4 boneMatrix;
	VECTOR vecProj;

	for(int i=0; i<10; i++)
	{
		if(m_bObjectSlotUsed[i] && m_pAttachedObjects[i])
		{
			if(!m_pAttachedObjects[i]->m_pEntity)
				continue;

			CObject *pObject = m_pAttachedObjects[i];
			if(!pObject)
				continue;
			
			if(IsAdded())
			{
				if(pObject->GetCollisionChecking()) {
					pObject->SetCollisionChecking(false);
				}

				// TODO: will be make it
			}
			else
			{
				pObject->TeleportTo(0.0f, 0.0f, 0.0f);
			}
		}
	}
}

uint16_t CPlayerPed::GetKeys(uint16_t *lrAnalog, uint16_t *udAnalog)
{
	*lrAnalog = LocalPlayerKeys.wKeyLR;
	*udAnalog = LocalPlayerKeys.wKeyUD;

	uint16_t wRet = 0;

	// KEY_ANALOG_RIGHT
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_ANALOG_RIGHT]) wRet |= 1;
	wRet <<= 1;
	// KEY_ANALOG_LEFT
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_ANALOG_LEFT]) wRet |= 1;
	wRet <<= 1;
	// KEY_ANALOG_DOWN
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_ANALOG_DOWN]) wRet |= 1;
	wRet <<= 1;
	// KEY_ANALOG_UP
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_ANALOG_UP]) wRet |= 1;
	wRet <<= 1;
	// KEY_WALK
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_WALK]) wRet |= 1;
	wRet <<= 1;
	// KEY_SUBMISSION
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_SUBMISSION]) wRet |= 1;
	wRet <<= 1;
	// KEY_WALK
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_WALK]) wRet |= 1;
	wRet <<= 1;
	// KEY_SUBMISSION
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_SUBMISSION]) wRet |= 1;
	wRet <<= 1;
	// KEY_LOOK_LEFT
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_LEFT]) wRet |= 1;
	wRet <<= 1;
	// KEY_HANDBRAKE
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_HANDBRAKE]) wRet |= 1;
	wRet <<= 1;
	// KEY_LOOK_RIGHT
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_LOOK_RIGHT]) wRet |= 1;
	wRet <<= 1;
	// KEY_JUMP
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_JUMP]) wRet |= 1;
	wRet <<= 1;
	// KEY_SECONDARY_ATTACK
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK]) wRet |= 1;
	wRet <<= 1;
	// KEY_SPRINT
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_SPRINT]) wRet |= 1;
	wRet <<= 1;
	// KEY_FIRE
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE]) wRet |= 1;
	wRet <<= 1;
	// KEY_CROUCH
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH]) wRet |= 1;
	wRet <<= 1;
	// KEY_ACTION
	if(LocalPlayerKeys.bKeys[ePadKeys::KEY_ACTION]) wRet |= 1;

	memset(LocalPlayerKeys.bKeys, 0, ePadKeys::SIZE);

	return wRet;
}

void CPlayerPed::SetKeys(uint16_t wKeys, uint16_t lrAnalog, uint16_t udAnalog)
{
	PAD_KEYS *pad = &RemotePlayerKeys[m_bytePlayerNumber];

	// LEFT/RIGHT
	pad->wKeyLR = lrAnalog;
	// UP/DOWN
	pad->wKeyUD = udAnalog;

	// KEY_ACTION
	pad->bKeys[ePadKeys::KEY_ACTION] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_CROUCH
	pad->bKeys[ePadKeys::KEY_CROUCH] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_FIRE
	pad->bKeys[ePadKeys::KEY_FIRE] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_SPRINT
	pad->bKeys[ePadKeys::KEY_SPRINT] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_SECONDARY_ATTACK
	pad->bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_JUMP
	pad->bKeys[ePadKeys::KEY_JUMP] = (wKeys & 1);
	if(!pad->bKeys[ePadKeys::KEY_JUMP]) pad->bIgnoreJump = false;
	wKeys >>= 1;
	// KEY_LOOK_RIGHT
	pad->bKeys[ePadKeys::KEY_LOOK_RIGHT] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_HANDBRAKE
	pad->bKeys[ePadKeys::KEY_HANDBRAKE] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_LOOK_LEFT
	pad->bKeys[ePadKeys::KEY_LOOK_LEFT] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_SUBMISSION
	pad->bKeys[ePadKeys::KEY_SUBMISSION] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_WALK
	pad->bKeys[ePadKeys::KEY_WALK] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_ANALOG_UP
	pad->bKeys[ePadKeys::KEY_ANALOG_UP] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_ANALOG_DOWN
	pad->bKeys[ePadKeys::KEY_ANALOG_DOWN] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_ANALOG_LEFT
	pad->bKeys[ePadKeys::KEY_ANALOG_LEFT] = (wKeys & 1);
	wKeys >>= 1;
	// KEY_ANALOG_RIGHT
	pad->bKeys[ePadKeys::KEY_ANALOG_RIGHT] = (wKeys & 1);

	return;
}


#pragma once

#include "aimstuff.h"
#include "object.h"

class CPlayerPed : public CEntity
{
public:
	CPlayerPed();	
	CPlayerPed(uint8_t bytePlayerNumber, int iSkin, float fX, float fY, float fZ, float fRotation);
	~CPlayerPed();

	void Destroy();

	bool IsInVehicle();
	bool IsAPassenger();
	VEHICLE_TYPE* GetGtaVehicle();
	void RemoveFromVehicleAndPutAt(float fX, float fY, float fZ);
	void SetInitialState();
	void SetHealth(float fHealth);
	void SetArmour(float fArmour);
	float GetHealth();
	float GetArmour();
	void TogglePlayerControllable(bool bToggle);
	void SetModelIndex(unsigned int uiModel);

	void SetInterior(uint8_t byteID);

	void PutDirectlyInVehicle(int iVehicleID, int iSeat);
	void EnterVehicle(int iVehicleID, bool bPassenger);
	void ExitCurrentVehicle();
	int GetCurrentVehicleID();
	int GetVehicleSeatID();

	ENTITY_TYPE* GetEntityUnderPlayer();

	void GiveWeapon(int iWeaponID, int iAmmo);
	uint8_t GetCurrentWeapon();
	void SetArmedWeapon(int iWeaponID);
	void ClearAllWeapons();
	
	void DestroyFollowPedTask();
	void ResetDamageEntity();

	void RestartIfWastedAt(VECTOR *vecRestart, float fRotation);
	void ForceTargetRotation(float fRotation);
	uint8_t GetActionTrigger();
	bool IsDead();

	uint16_t GetKeys(uint16_t *lrAnalog, uint16_t *udAnalog);
	void SetKeys(uint16_t wKeys, uint16_t lrAnalog, uint16_t udAnalog);

	void SetMoney(int iAmount);
	void ShowMarker(uint32_t iMarkerColorID);
	void HideMarker();
	void SetFightingStyle(int iStyle);
	void SetRotation(float fRotation);
	void ApplyAnimation( char *szAnimName, char *szAnimFile, float fT, int opt1, int opt2, int opt3, int opt4, int iUnk );
	void GetBonePosition(int iBoneID, VECTOR* vecOut);
	void GetBonePosition(VECTOR &out, unsigned int boneid, bool update);
	// roflan
	void FindDeathReasonAndResponsiblePlayer(PLAYERID *nPlayer);

	void SetControllable(bool bToggle);
	void ClumpUpdateAnimations(float step, int flag);

	bool IsHaveAttachedObject();
	void RemoveAllAttachedObjects();
	void UpdateAttachedObject(bool create, uint32_t index, uint32_t model, uint32_t bone, VECTOR vecOffset, VECTOR vecRotation, VECTOR vecScale, uint32_t materialcolor1, uint32_t materialcolor2);
	bool GetObjectSlotState(int iObjectIndex);
	void ProcessAttachedObjects();
	bool GetBoneMatrix(MATRIX4X4 *matOut, int iBoneID);
	void ResetAttachedObjects();

	PED_TYPE * GetGtaActor() { return m_pPed; };

public:
	PED_TYPE*	m_pPed;
	uint8_t		m_bytePlayerNumber;
	uint32_t	m_dwArrow;

	ATTACHED_OBJECT 	m_AttachedObjectInfo[10];
	bool 				m_bObjectSlotUsed[10];
	CObject* 			m_pAttachedObjects[10];
};
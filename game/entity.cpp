#include "../main.h"
#include "game.h"
#include "net/netgame.h"
#include "chatwindow.h"

#include <cmath>

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CChatWindow *pChatWindow;

// 0.3.7
void CEntity::GetMatrix(PMATRIX4X4 Matrix)
{
	if (!m_pEntity) {
		return;
	}

	MATRIX4X4* mat = *(MATRIX4X4**)((uintptr_t)m_pEntity + 20);
	if(!mat) {
		return;
	}

	Matrix->right.X = mat->right.X;
	Matrix->right.Y = mat->right.Y;
	Matrix->right.Z = mat->right.Z;

	Matrix->up.X = mat->up.X;
	Matrix->up.Y = mat->up.Y;
	Matrix->up.Z = mat->up.Z;

	Matrix->at.X = mat->at.X;
	Matrix->at.Y = mat->at.Y;
	Matrix->at.Z = mat->at.Z;

	Matrix->pos.X = mat->pos.X;
	Matrix->pos.Y = mat->pos.Y;
	Matrix->pos.Z = mat->pos.Z;
}

// 0.3.7
void CEntity::SetMatrix(MATRIX4X4 Matrix)
{
	if (!m_pEntity) {
		return;
	}

	MATRIX4X4* mat = *(MATRIX4X4**)((uintptr_t)m_pEntity + 20);
	if(!mat) {
		return;
	}
  
  	mat->right.X = Matrix.right.X;
	mat->right.Y = Matrix.right.Y;
	mat->right.Z = Matrix.right.Z;

	mat->up.X = Matrix.up.X;
	mat->up.Y = Matrix.up.Y;
	mat->up.Z = Matrix.up.Z;

	mat->at.X = Matrix.at.X;
	mat->at.Y = Matrix.at.Y;
	mat->at.Z = Matrix.at.Z;

	mat->pos.X = Matrix.pos.X;
	mat->pos.Y = Matrix.pos.Y;
	mat->pos.Z = Matrix.pos.Z;
}

// 0.3.7
void CEntity::GetMoveSpeedVector(PVECTOR Vector)
{
	Vector->X = *(float *)((uintptr_t)m_pEntity + 72);
	Vector->Y = *(float *)((uintptr_t)m_pEntity + 76);
	Vector->Z = *(float *)((uintptr_t)m_pEntity + 80);
}

// 0.3.7
void CEntity::SetMoveSpeedVector(VECTOR Vector)
{
	*(float *)((uintptr_t)m_pEntity + 72) = Vector.X;
	*(float *)((uintptr_t)m_pEntity + 76) = Vector.Y;
	*(float *)((uintptr_t)m_pEntity + 80) = Vector.Z;
}

void CEntity::GetTurnSpeedVector(PVECTOR Vector)
{
	Vector->X = *(float *)((uintptr_t)m_pEntity + 84);
	Vector->Y = *(float *)((uintptr_t)m_pEntity + 88);
	Vector->Z = *(float *)((uintptr_t)m_pEntity + 92);
}

void CEntity::SetTurnSpeedVector(VECTOR Vector)
{
	*(float *)((uintptr_t)m_pEntity + 84) = Vector.X;
	*(float *)((uintptr_t)m_pEntity + 88) = Vector.Y;
	*(float *)((uintptr_t)m_pEntity + 92) = Vector.Z;
}

// 0.3.7
uint16_t CEntity::GetModelIndex()
{
	return m_pEntity->nModelIndex;
}

// 0.3.7
bool CEntity::IsAdded()
{
	if(m_pEntity)
	{
		if(m_pEntity->vtable == g_GTASAAdr+0x667D24) {
			return false;
		}

		if(m_pEntity->dwUnkModelRel) {
			return true;
		}
	}

	return false;
}

// 0.3.7
bool CEntity::SetModelIndex(unsigned int uiModel)
{
	if(!m_pEntity) return false;

	int iTryCount = 0;
	if(!pGame->IsModelLoaded(uiModel) && !IsValidModel(uiModel))
	{
		pGame->RequestModel(uiModel);
		pGame->LoadRequestedModels();
		while(!pGame->IsModelLoaded(uiModel))
		{
			usleep(1000);
			if(iTryCount > 200)
			{
				if(pChatWindow) {
					pChatWindow->AddDebugMessage("Warning: Model %u wouldn't load in time!", uiModel);
				}

				return false;
			}

			iTryCount++;
		}
	}

	// CEntity::DeleteRWObject()
	(( void (*)(ENTITY_TYPE*))(*(void**)(m_pEntity->vtable+0x24)))(m_pEntity);
	m_pEntity->nModelIndex = uiModel;

	// CEntity::SetModelIndex()
	(( void (*)(ENTITY_TYPE*, unsigned int))(*(void**)(m_pEntity->vtable+0x18)))(m_pEntity, uiModel);

	return true;
}

// 0.3.7
void CEntity::TeleportTo(float fX, float fY, float fZ)
{
	if(m_pEntity && m_pEntity->vtable != (g_GTASAAdr+0x667D24)) /* CPlaceable */
	{
		uint16_t modelIndex = m_pEntity->nModelIndex;
		if(	modelIndex != TRAIN_PASSENGER_LOCO &&
			modelIndex != TRAIN_FREIGHT_LOCO &&
			modelIndex != TRAIN_TRAM)
			(( void (*)(ENTITY_TYPE*, float, float, float, bool))(*(void**)(m_pEntity->vtable+0x3C)))(m_pEntity, fX, fY, fZ, 0);
		else
			ScriptCommand(&put_train_at, m_dwGTAId, fX, fY, fZ);
	}
}

float CEntity::GetDistanceFromCamera()
{
	MATRIX4X4 matEnt;

	if(!m_pEntity || m_pEntity->vtable == g_GTASAAdr+0x667D24 /* CPlaceable */)
		return 100000.0f;

	this->GetMatrix(&matEnt);
	
	float tmpX = (matEnt.pos.X - *(float*)(g_GTASAAdr+0x9528D4));
	float tmpY = (matEnt.pos.Y - *(float*)(g_GTASAAdr+0x9528D8));
	float tmpZ = (matEnt.pos.Z - *(float*)(g_GTASAAdr+0x9528DC));

	return sqrt( tmpX*tmpX + tmpY*tmpY + tmpZ*tmpZ );
}

float CEntity::GetDistanceFromLocalPlayerPed()
{
	MATRIX4X4	matFromPlayer;
	MATRIX4X4	matThis;
	float 		fSX, fSY, fSZ;

	CPlayerPed *pLocalPlayerPed = pGame->FindPlayerPed();
	CLocalPlayer *pLocalPlayer  = nullptr;

	if(!pLocalPlayerPed) return 10000.0f;

	GetMatrix(&matThis);

	if(pNetGame)
	{
		pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
		if(pLocalPlayer && (pLocalPlayer->IsSpectating() || pLocalPlayer->IsInRCMode()))
		{
			pGame->GetCamera()->GetMatrix(&matFromPlayer);
		}
		else
		{
			pLocalPlayerPed->GetMatrix(&matFromPlayer);
		}
	}
	else
	{
		pLocalPlayerPed->GetMatrix(&matFromPlayer);
	}

	fSX = (matThis.pos.X - matFromPlayer.pos.X) * (matThis.pos.X - matFromPlayer.pos.X);
	fSY = (matThis.pos.Y - matFromPlayer.pos.Y) * (matThis.pos.Y - matFromPlayer.pos.Y);
	fSZ = (matThis.pos.Z - matFromPlayer.pos.Z) * (matThis.pos.Z - matFromPlayer.pos.Z);

	return (float)sqrt(fSX + fSY + fSZ);
}

float CEntity::GetDistanceFromPoint(float X, float Y, float Z)
{
	MATRIX4X4	matThis;
	float		fSX,fSY,fSZ;

	GetMatrix(&matThis);
	fSX = (matThis.pos.X - X) * (matThis.pos.X - X);
	fSY = (matThis.pos.Y - Y) * (matThis.pos.Y - Y);
	fSZ = (matThis.pos.Z - Z) * (matThis.pos.Z - Z);
	
	return (float)sqrt(fSX + fSY + fSZ);
}
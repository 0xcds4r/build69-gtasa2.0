#include "../main.h"
#include "game.h"
#include "net/netgame.h"
#include "chatwindow.h"

#include <cmath>

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CChatWindow *pChatWindow;

// 0.3.7
void CEntity::Add()
{
	if(!m_pEntity || m_pEntity->vtable == 0x667D24) {
		return;
	}

	if(!*(uintptr_t*)((uintptr_t)m_pEntity + 0xB8)) 
	{
		VECTOR vec = {0.0f,0.0f,0.0f};
		SetMoveSpeedVector(vec);
		SetTurnSpeedVector(vec);

		// CWorld::Add
		((void(*)(uintptr_t))(g_GTASAAdr+0x4233C8+1))((uintptr_t)m_pEntity);

		MATRIX4X4 mat;
		GetMatrix(&mat);
		TeleportTo(mat.pos.X, mat.pos.Y, mat.pos.Z);
	}
}

// 0.3.7
void CEntity::Remove()
{
	if(!m_pEntity || m_pEntity->vtable == 0x667D24) {
		return;
	}

	if(*(uintptr_t*)((uintptr_t)m_pEntity + 0xB8)) 
	{
		// CWorld::Remove
		((void(*)(uintptr_t))(g_GTASAAdr+0x4232BC+1))((uintptr_t)m_pEntity);
	}
}

// 0.3.7
void CEntity::UpdateRwMatrixAndFrame()
{
	if(m_pEntity && m_pEntity->vtable != g_GTASAAdr+0x667D24)
	{
		if(*(uintptr_t *)((uintptr_t)m_pEntity + 0x18))
		{
			MATRIX4X4* mat = *(MATRIX4X4**)((uintptr_t)m_pEntity + 20);

			if(mat)
			{
				uintptr_t pRwMatrix = *(uintptr_t*)((*(uintptr_t *)((uintptr_t)m_pEntity + 0x18)) + 4) + 0x10;
				
				// CMatrix::UpdateRwMatrix
				((void (*) (MATRIX4X4*, uintptr_t))(g_GTASAAdr+0x44EDEE))(mat, pRwMatrix);

				// CEntity::UpdateRwFrame
				((void (*) (ENTITY_TYPE*))(g_GTASAAdr+0x3EBFE8))(m_pEntity);
			}
		}
	}
}

// 0.3.7
void CEntity::UpdateMatrix(MATRIX4X4 mat)
{
	if(m_pEntity)
	{
		MATRIX4X4* matUk = *(MATRIX4X4**)((uintptr_t)m_pEntity + 20);
		MATRIX4X4 mat = *(MATRIX4X4*)((uintptr_t)m_pEntity + 20);

		if(matUk)
		{
			// CPhysical::Remove
			((void (*)(ENTITY_TYPE*))(*(uintptr_t*)(m_pEntity->vtable + 0x10)))(m_pEntity);

			SetMatrix(mat);
			UpdateRwMatrixAndFrame();

			// CPhysical::Add
			((void (*)(ENTITY_TYPE*))(*(uintptr_t*)(m_pEntity->vtable + 0x8)))(m_pEntity);
		}
	}
}

// 0.3.7
void CEntity::Render()
{
	int iModel = GetModelIndex();
	if(iModel >= 400 && iModel <= 611 && *(uintptr_t *)((uintptr_t)m_pEntity + 0x18))
	{
		// CVisibilityPlugins::SetupVehicleVariables
		((void (__fastcall *)(uintptr_t))(g_GTASAAdr + 0x5D4B41))(*(uintptr_t *)((uintptr_t)m_pEntity + 0x18));
	}

	// CEntity::PreRender
	(( void (*)(ENTITY_TYPE*))(*(void**)(m_pEntity->vtable+0x48)))(m_pEntity);

	// CRenderer::RenderOneNonRoad
	(( void (*)(ENTITY_TYPE*))(g_GTASAAdr+0x4102BC))(m_pEntity);
}

// 0.3.7
bool CEntity::IsValidMatrixPointer(MATRIX4X4* mat)
{
	if(!pNetGame) {
		return true;
	}

	if(!pNetGame->HasGameLogic()) {
		return true;
	}

	VECTOR vecMin = pNetGame->m_vecWorldBordersFrom;
	VECTOR vecMax = pNetGame->m_vecWorldBordersTo;

	if(isnan(mat->right.X) || isnan(mat->right.Y) || isnan(mat->right.Z) || isnan(mat->up.X) || isnan(mat->up.Y) || isnan(mat->up.Z)) {
		return false;
	}

	if(isnan(mat->at.X) || isnan(mat->at.Y) || isnan(mat->at.Z)) {
		return false;
	}

	if(std::isinf(mat->right.X) || std::isinf(mat->right.Y) || std::isinf(mat->right.Z) || std::isinf(mat->up.X) || std::isinf(mat->up.Y) || std::isinf(mat->up.Z)) {
		return false;
	}

	if(std::isinf(mat->at.X) || std::isinf(mat->at.Y) || std::isinf(mat->at.Z)) {
		return false;
	}

	if(mat->right.X > vecMax.X || mat->right.X < vecMin.X || mat->right.Y > vecMax.Y || mat->right.Y < vecMin.Y || mat->right.Z > vecMax.Z || mat->right.Z < vecMin.Z) {
		return false;
	}

	if(mat->at.X > vecMax.X || mat->at.X < vecMin.X || mat->at.Y > vecMax.Y || mat->at.Y < vecMin.Y || mat->at.Z > vecMax.Z || mat->at.Z < vecMin.Z) {
		return false;
	}

	if(mat->up.X > vecMax.X || mat->up.X < vecMin.X || mat->up.Y > vecMax.Y || mat->up.Y < vecMin.Y || mat->up.Z > vecMax.Z || mat->up.Z < vecMin.Z) {
		return false;
	}

	return true;
}

// 0.3.7
bool CEntity::IsValidMatrix(MATRIX4X4 mat)
{
	if(!pNetGame) {
		return true;
	}

	if(!pNetGame->HasGameLogic()) {
		return true;
	}

	VECTOR vecMin = pNetGame->m_vecWorldBordersFrom;
	VECTOR vecMax = pNetGame->m_vecWorldBordersTo;

	if(isnan(mat.right.X) || isnan(mat.right.Y) || isnan(mat.right.Z) || isnan(mat.up.X) || isnan(mat.up.Y) || isnan(mat.up.Z)) {
		return false;
	}

	if(isnan(mat.at.X) || isnan(mat.at.Y) || isnan(mat.at.Z)) {
		return false;
	}

	if(std::isinf(mat.right.X) || std::isinf(mat.right.Y) || std::isinf(mat.right.Z) || std::isinf(mat.up.X) || std::isinf(mat.up.Y) || std::isinf(mat.up.Z)) {
		return false;
	}

	if(std::isinf(mat.at.X) || std::isinf(mat.at.Y) || std::isinf(mat.at.Z)) {
		return false;
	}

	if(mat.right.X > vecMax.X || mat.right.X < vecMin.X || mat.right.Y > vecMax.Y || mat.right.Y < vecMin.Y || mat.right.Z > vecMax.Z || mat.right.Z < vecMin.Z) {
		return false;
	}

	if(mat.at.X > vecMax.X || mat.at.X < vecMin.X || mat.at.Y > vecMax.Y || mat.at.Y < vecMin.Y || mat.at.Z > vecMax.Z || mat.at.Z < vecMin.Z) {
		return false;
	}

	if(mat.up.X > vecMax.X || mat.up.X < vecMin.X || mat.up.Y > vecMax.Y || mat.up.Y < vecMin.Y || mat.up.Z > vecMax.Z || mat.up.Z < vecMin.Z) {
		return false;
	}

	return true;
}

// 0.3.7
bool CEntity::IsValidVector(VECTOR vecSpeed)
{
	if(!pNetGame) {
		return true;
	}

	if(!pNetGame->HasGameLogic()) {
		return true;
	}

	VECTOR vecMin = pNetGame->m_vecWorldBordersFrom;
	VECTOR vecMax = pNetGame->m_vecWorldBordersTo;

	if(isnan(vecMin.X) || isnan(vecMin.Y) || isnan(vecMin.Z) || isnan(vecMax.X) || isnan(vecMax.Y) || isnan(vecMax.Z)) {
		return false;
	}

	if(std::isinf(vecMin.X) || std::isinf(vecMin.Y) || std::isinf(vecMin.Z) || std::isinf(vecMax.X) || std::isinf(vecMax.Y) || std::isinf(vecMax.Z)) {
		return false;
	}

	if(vecSpeed.X < vecMin.X || vecSpeed.Y < vecMin.Y || vecSpeed.Z < vecMin.Z) {
		return false;
	}

	if(vecSpeed.X > vecMax.X || vecSpeed.Y > vecMax.Y || vecSpeed.Z > vecMax.Z) {
		return false;
	}

	return true;
}

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

	/*if(!IsValidMatrix(Matrix)) 
	{
		FLog("Invalid Matrix >> %f %f %f | %f %f %f | %f %f %f | %f %f %f", 
			Matrix.right.X, Matrix.right.Y, Matrix.right.Z, 
			Matrix.up.X, Matrix.up.Y, Matrix.up.Z, 
			Matrix.at.X, Matrix.at.Y, Matrix.at.Z, 
			Matrix.pos.X, Matrix.pos.Y, Matrix.pos.Z);

		return;
	}*/
  
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
	if(!m_pEntity) {
		return;
	}

	VECTOR vecSpeed = {
		*(float *)((uintptr_t)m_pEntity + 72),
		*(float *)((uintptr_t)m_pEntity + 76),
		*(float *)((uintptr_t)m_pEntity + 80)
	};

	Vector->X = vecSpeed.X;
	Vector->Y = vecSpeed.Y;
	Vector->Z = vecSpeed.Z;
}

// 0.3.7
void CEntity::SetMoveSpeedVector(VECTOR Vector)
{
	if(!m_pEntity) {
		return;
	}

	if(!IsValidVector(Vector)) {
		FLog("[1] Invalid Speed Vector >> %f %f %f", 
			Vector.X, Vector.Y, Vector.Z);

		return;
	}

	*(float *)((uintptr_t)m_pEntity + 72) = Vector.X;
	*(float *)((uintptr_t)m_pEntity + 76) = Vector.Y;
	*(float *)((uintptr_t)m_pEntity + 80) = Vector.Z;
}

void CEntity::GetTurnSpeedVector(PVECTOR Vector)
{
	if(!m_pEntity) {
		return;
	}

	VECTOR vecSpeed = {
		*(float *)((uintptr_t)m_pEntity + 84),
		*(float *)((uintptr_t)m_pEntity + 88),
		*(float *)((uintptr_t)m_pEntity + 92)
	};

	Vector->X = vecSpeed.X;
	Vector->Y = vecSpeed.Y;
	Vector->Z = vecSpeed.Z;
}

void CEntity::SetTurnSpeedVector(VECTOR Vector)
{
	if(!m_pEntity) {
		return;
	}

	if(!IsValidVector(Vector)) {
		FLog("[2] Invalid Speed Vector >> %f %f %f", 
			Vector.X, Vector.Y, Vector.Z);

		return;
	}

	*(float *)((uintptr_t)m_pEntity + 84) = Vector.X;
	*(float *)((uintptr_t)m_pEntity + 88) = Vector.Y;
	*(float *)((uintptr_t)m_pEntity + 92) = Vector.Z;
}

// 0.3.7
uint16_t CEntity::GetModelIndex()
{
	if(!m_pEntity || m_pEntity->vtable == g_GTASAAdr+0x667D24) {
		return 0;
	}

	return *(uint16_t*)((uintptr_t)m_pEntity + 0x26);
}

// 0.3.7
bool CEntity::IsAdded()
{
	if(m_pEntity)
	{
		if(m_pEntity->vtable == g_GTASAAdr+0x667D24) {
			return false;
		}

		if(*(uintptr_t*)((uintptr_t)m_pEntity + 0xB8)) { 
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
	*(uint16_t*)((uintptr_t)m_pEntity + 0x26) = uiModel;

	// CEntity::SetModelIndex()
	(( void (*)(ENTITY_TYPE*, unsigned int))(*(void**)(m_pEntity->vtable+0x18)))(m_pEntity, uiModel);

	return true;
}

// 0.3.7
void CEntity::TeleportTo(float fX, float fY, float fZ)
{
	/*if(!IsValidVector({fX, fY, fZ})) {
		FLog("[3] Invalid Vector >> %f %f %f", 
			fX, fY, fZ);

		return;
	}*/

	if(m_pEntity && m_pEntity->vtable != (g_GTASAAdr+0x667D24)) /* CPlaceable */
	{
		uint16_t modelIndex = *(uint16_t*)((uintptr_t)m_pEntity + 0x26);
		if(	modelIndex != TRAIN_PASSENGER_LOCO &&
			modelIndex != TRAIN_FREIGHT_LOCO &&
			modelIndex != TRAIN_TRAM) 
		{
			(( void (*)(ENTITY_TYPE*, float, float, float, bool))(*(void**)(m_pEntity->vtable+0x3C)))(m_pEntity, fX, fY, fZ, 0);
		}	
		else
		{
			ScriptCommand(&put_train_at, m_dwGTAId, fX, fY, fZ);
		}
	}
}

float CEntity::GetDistanceFromCamera()
{
	MATRIX4X4 matEnt;

	if(!m_pEntity || m_pEntity->vtable == g_GTASAAdr+0x667D24 /* CPlaceable */) {
		return 100000.0f;
	}

	this->GetMatrix(&matEnt);
	
	CAMERA_AIM *pCam = GameGetInternalAim();
	if(!pCam) {
		return 100000.0f;
	}

	float tmpX = (matEnt.pos.X - pCam->pos1x); // *(float*)(g_GTASAAdr+0x9528D4)
	float tmpY = (matEnt.pos.Y - pCam->pos1y); // *(float*)(g_GTASAAdr+0x9528D8)
	float tmpZ = (matEnt.pos.Z - pCam->pos1z); // *(float*)(g_GTASAAdr+0x9528DC)

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

uintptr_t CEntity::GetRWObject()
{
	if(!m_pEntity) {
		return 0;
	}

	return *(uintptr_t *)((uintptr_t)m_pEntity + 0x18);
}

void CEntity::SetGravityProcessing(bool bState)
{
	if(!m_pEntity || m_pEntity->vtable == g_GTASAAdr+0x667D24) {
		return;
	}

	if(bState)
	{
		*(uint32_t *)((uintptr_t)m_pEntity + 28) |= 0x80000002;
	}
	else
	{
		*(uint32_t *)((uintptr_t)m_pEntity + 28) &= 0x7FFFFFFD;
	}
}

bool CEntity::GetCollisionChecking()
{
	if(!m_pEntity || m_pEntity->vtable == g_GTASAAdr+0x667D24) {
		return true;
	}

	uintptr_t* ptrTmp = (uintptr_t*)&m_pEntity; 

	return ptrTmp[7];
}

void CEntity::SetCollisionChecking(bool bState)
{
	if(!m_pEntity || m_pEntity->vtable == g_GTASAAdr+0x667D24) {
		return;
	}

	if(bState)
	{
		uintptr_t enablePtr = (uintptr_t)0x180000a3;
		*(uint32_t *)((uintptr_t)m_pEntity + 28) = (uint32_t)enablePtr;
	}
	else
	{
		uintptr_t disablePtr = (uintptr_t)0x180000a2;
   		*(uint32_t *)((uintptr_t)m_pEntity + 28) = (uint32_t)disablePtr;
	}
}
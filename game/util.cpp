#include "../main.h"
#include "game.h"
#include "net/netgame.h"

extern CNetGame *pNetGame;

#define PI 3.14159265

uintptr_t dwPlayerPedPtrs[PLAYER_PED_SLOTS];
bool bTextDrawTextureSlotState[200];
uintptr_t TextDrawTexture[200];

extern char* PLAYERS_REALLOC;

PED_TYPE* GamePool_FindPlayerPed()
{
	return ((PED_TYPE*(*)(int))(g_GTASAAdr + 0x40B288 + 1))(-1);
}

PED_TYPE* GamePool_Ped_GetAt(int iID)
{
	return (( PED_TYPE* (*)(int))(g_GTASAAdr+0x483D49))(iID);
}

int GamePool_Ped_GetIndex(PED_TYPE *pActor)
{
    return (( int (*)(uintptr_t))(g_GTASAAdr+0x483D3A+1))((uintptr_t)pActor);
}

VEHICLE_TYPE *GamePool_Vehicle_GetAt(int iID)
{
	return (( VEHICLE_TYPE* (*)(int))(g_GTASAAdr+0x483D2F))(iID);
}

int GamePool_Vehicle_GetIndex(VEHICLE_TYPE *pVehicle)
{
    return (( int (*)(VEHICLE_TYPE*))(g_GTASAAdr+0x483D21))(pVehicle);
}

ENTITY_TYPE *GamePool_Object_GetAt(int iID)
{
	ENTITY_TYPE* (*GetPoolObj)(int iID);
	*(void **) (&GetPoolObj) = (void*)(g_GTASAAdr+0x483D63);
	return (GetPoolObj)(iID);
} 

int LineOfSight(VECTOR* start, VECTOR* end, void* colpoint, uintptr_t ent,
	char buildings, char vehicles, char peds, char objects, char dummies, bool seeThrough, bool camera, bool unk)
{
	return 0;
}

// 0.3.7
bool IsPedModel(unsigned int iModelID)
{
	if(iModelID < 0 || iModelID > 20000) return false;
    uintptr_t *dwModelArray = (uintptr_t*)(g_GTASAAdr+0x91DCB8);
    
    uintptr_t ModelInfo = dwModelArray[iModelID];
    if(ModelInfo && *(uintptr_t*)ModelInfo == (uintptr_t)(g_GTASAAdr+0x667668/*CPedModelInfo vtable*/))
        return true;

    return false;
}

// 0.3.7
bool IsValidModel(unsigned int uiModelID)
{
    if(uiModelID < 0 || uiModelID > 20000) return false;
    uintptr_t *dwModelArray = (uintptr_t*)(g_GTASAAdr+0x91DCB8);

    uintptr_t dwModelInfo = dwModelArray[uiModelID];
    if(dwModelInfo && *(uintptr_t*)(dwModelInfo+0x34/*pRwObject*/)) {
        return true;
    }

    return false;
}

uint16_t GetModelReferenceCount(int nModelIndex)
{
	if(nModelIndex < 0 || nModelIndex > 20000) return 0;

	uintptr_t *dwModelarray = (uintptr_t*)(g_GTASAAdr+0x91DCB8);
	uint8_t *pModelInfoStart = (uint8_t*)dwModelarray[nModelIndex];
	
	return *(uint16_t*)(pModelInfoStart+0x1E);
}

void InitPlayerPedPtrRecords()
{
	memset(&dwPlayerPedPtrs[0], 0, sizeof(uintptr_t) * PLAYER_PED_SLOTS);
}

void SetPlayerPedPtrRecord(uint8_t bytePlayer, uintptr_t dwPedPtr)
{
	dwPlayerPedPtrs[bytePlayer] = dwPedPtr;
}

uint8_t FindPlayerNumFromPedPtr(uintptr_t dwPedPtr)
{
	uint8_t x = 0;
	while(x != PLAYER_PED_SLOTS)
	{
		if(dwPlayerPedPtrs[x] == dwPedPtr) return x;
		x++;
	}

	return 0;
}

uintptr_t GetTexture(const char* texture)
{
	// FLog("GetTexture: %s", texture);

	// GetTexture
	uintptr_t pRwTexture = (( uintptr_t (*)(const char*))(g_GTASAAdr+0x1E9CE5))(texture);

	if(pRwTexture) 
	{
		int iTextureCount = *(int*)(pRwTexture + 0x54);
		iTextureCount++;
		*(int*)(pRwTexture + 0x54) = iTextureCount;
	}

	return pRwTexture;
}

uintptr_t LoadTextureFromDB(const char* dbname, const char* texture)
{
	// TextureDatabaseRuntime::GetDatabase
	uintptr_t db_handle = (( uintptr_t (*)(const char*))(g_GTASAAdr+0x1BF530+1))(dbname);

	if(!db_handle)
	{
		FLog("Error: Database not found! (%s)", dbname);
		return 0;
	}
	
	// TextureDatabaseRuntime::Register
	(( void (*)(uintptr_t))(g_GTASAAdr+0x1E9BC8+1))(db_handle);
	uintptr_t tex = GetTexture(texture);

	if(!tex) FLog("Error: Texture (%s) not found in database (%s)", dbname, texture);

	// TextureDatabaseRuntime::Unregister
	(( void (*)(uintptr_t))(g_GTASAAdr+0x1E9C80+1))(db_handle);

	return tex;
}

void DefinedState2d()
{
	return (( void (*)())(g_GTASAAdr+0x5D0C65))();
}

void SetScissorRect(void* pRect)
{
	return ((void(*)(void*))(g_GTASAAdr+0x2B3E55))(pRect);
}

float DegToRad(float fDegrees)
{
    if (fDegrees > 360.0f || fDegrees < 0.0f) return 0.0f;
    
    if (fDegrees > 180.0f) return (float)(-(PI - (((fDegrees - 180.0f) * PI) / 180.0f)));
    else return (float)((fDegrees * PI) / 180.0f);
}

// 0.3.7
float FloatOffset(float f1, float f2)
{   
    if(f1 >= f2) return f1 - f2;
    else return (f2 - f1);
}

uintptr_t GetModelInfoByID(int iModelID)
{
	if (iModelID < 0 || iModelID > 20000) {
		return false;
	}

	uintptr_t *dwModelArray = (uintptr_t*)(g_GTASAAdr+0x91DCB8);
	return dwModelArray[iModelID];
}

uintptr_t ModelInfoCreateInstance(int iModel)
{
	uintptr_t modelInfo = GetModelInfoByID(iModel);

	if (modelInfo) {
		return ((uintptr_t(*)(uintptr_t))*(uintptr_t*)(*(uintptr_t*)modelInfo + 0x2C))(modelInfo);
	}
	
	return 0;
}

void RenderClumpOrAtomic(uintptr_t rwObject)
{
	if (rwObject)
	{
		if (*(uint8_t*)rwObject == 1)
		{
			((void(*)(uintptr_t))( *(uintptr_t*)(rwObject+0x48) ))(rwObject);
		} 
		else if (*(uint8_t*)rwObject == 2)
		{
			((void(*)(uintptr_t))(g_GTASAAdr + 0x2142DD))(rwObject);
		}
	}
}

float GetModelColSphereRadius(int iModel)
{
	uintptr_t modelInfo = GetModelInfoByID(iModel);

	if (modelInfo)
	{
		uintptr_t colModel = *(uintptr_t*)(modelInfo + 0x2C);

		if (colModel != 0) 
		{
			return *(float*)(colModel + 0x24);
		}
	}

	return 0.0f;
}

void GetModelColSphereVecCenter(int iModel, VECTOR* vec)
{
	uintptr_t modelInfo = GetModelInfoByID(iModel);

	if (modelInfo)
	{
		uintptr_t colModel = *(uintptr_t*)(modelInfo + 0x2C);

		if (colModel != 0) 
		{
			VECTOR* v = (VECTOR*)(colModel + 0x18);

			vec->X = v->X;
			vec->Y = v->Y;
			vec->Z = v->Z;
		}
	}
}

void DestroyAtomicOrClump(uintptr_t rwObject)
{
	if (rwObject)
	{
		int type = *(int*)(rwObject);

		if (type == 1)
		{
			// RpAtomicDestroy
			((void(*)(uintptr_t))(g_GTASAAdr + 0x2141ED))(rwObject); 

			uintptr_t parent = *(uintptr_t*)(rwObject + 4);

			if (parent)
			{
				// RwFrameDestroy
				((void(*)(uintptr_t))(g_GTASAAdr + 0x1D846D))(parent); 
			}

		}
		else if (type == 2)
		{
			// RpClumpDestroy
			((void(*)(uintptr_t))(g_GTASAAdr + 0x21460D))(rwObject);
		}
	}
}

void DrawTextureUV(uintptr_t texture, RECT* rect, uint32_t dwColor, float *uv)
{
	if (texture)
	{
		RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);

		// CSprite2d::Draw
		((void(*)(uintptr_t, RECT*, uint32_t*, float, float, float, float, float, float, float, float))(g_GTASAAdr + 0x5C95C1))(texture, rect, &dwColor, uv[0], uv[1], uv[2], uv[3], uv[4], uv[5], uv[6], uv[7]);
	}
}

uintptr_t LoadTextureFromTxd(const char *txd, const char *texture)
{
	uintptr_t pRwTexture = 0;

	int g_iTxdSlot = ((int (__fastcall *)(const char *))(g_GTASAAdr + 0x5D3E61))(txd); // CTxdStore::FindTxdSlot
	if(g_iTxdSlot == -1)
	{
		FLog("INVALID TXD: %s", txd);
		pRwTexture = 0;
	}
	else
	{
		((void (*)(void))(g_GTASAAdr + 0x5D4185))(); // CTxdStore::PushCurrentTxd
		((void (__fastcall *)(int, uint32_t))(g_GTASAAdr + 0x5D40F5))(g_iTxdSlot, 0);
		((void (__fastcall *)(uintptr_t *, const char *))(g_GTASAAdr + 0x5C8839))(&pRwTexture, texture); // CSprite2d::SetTexture

		if(!pRwTexture) {
			FLog("INVALID TEXTURE | TXD: %s TEXTURE: %s", txd, texture);
		}

		((void (*)(void))(g_GTASAAdr + 0x5D41C5))(); // CTxdStore::PopCurrentTxd
	}

	return pRwTexture;
}

void calculateAimVector(VECTOR *vec1, VECTOR *vec2)
{
	float f1;
  	float f2;
  	float f3;

  	f1 = atan2(vec1->X, vec1->Y) - 1.570796370506287;
  	f2 = sin(f1);
  	f3 = cos(f1);
  	vec2->X = vec1->Y * 0.0 - f3 * vec1->Z;
  	vec2->Y = f2 * vec1->Z - vec1->X * 0.0;
  	vec2->Z = f3 * vec1->X - f2 * vec1->Y;
}

uintptr_t LoadTexture(const char* texname)
{
	static char* texdb[] = { "samp", "gta3", "gta_int", "player", "txd" };

	for (int i = 0; i < 5; i++)
	{
		uintptr_t texture = LoadTextureFromDB(texdb[i], texname);
		if (texture != 0) {
			FLog("%s loaded from %s", texname, texdb[i]);
			return texture;
		}
	}

	FLog("Texture %s not found!", texname);
	return 0;
}

void DeleteRwTexture(uintptr_t pRwTexture)
{
	return (( void (*)(uintptr_t))(g_GTASAAdr+0x1DB765))(pRwTexture);
}

void ProjectMatrix(VECTOR* vecOut, MATRIX4X4* mat, VECTOR *vecPos)
{
	vecOut->X = mat->at.X * vecPos->Z + mat->up.X * vecPos->Y + mat->right.X * vecPos->X + mat->pos.X;
	vecOut->Y = mat->at.Y * vecPos->Z + mat->up.Y * vecPos->Y + mat->right.Y * vecPos->X + mat->pos.Y;
	vecOut->Z = mat->at.Z * vecPos->Z + mat->up.Z * vecPos->Y + mat->right.Z * vecPos->X + mat->pos.Z;
}

void RwMatrixOrthoNormalize(MATRIX4X4 *matIn, MATRIX4X4 *matOut)
{
	// RwMatrixOrthoNormalize
	((void (*)(MATRIX4X4*, MATRIX4X4*))(g_GTASAAdr + 0x1E34A0 + 1))(matIn, matOut);
}

void RwMatrixInvert(MATRIX4X4 *matOut, MATRIX4X4 *matIn)
{
	// RwMatrixInvert
	((void (*)(MATRIX4X4*, MATRIX4X4*))(g_GTASAAdr + 0x1E3A28 + 1))(matOut, matIn);
}

void RwMatrixRotate(MATRIX4X4 *mat, int axis, float angle)
{
	static float rMat[3][3] = 
	{
		{ 1.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f }
	};

	// RwMatrixRotate
	(( void (*)(MATRIX4X4*, float*, float, int))(g_GTASAAdr + 0x1E3974 + 1))(mat, rMat[axis], angle, 1);
}

void RwMatrixScale(MATRIX4X4 *matrix, VECTOR *vecScale)
{
	matrix->right.X *= vecScale->X;
	matrix->right.Y *= vecScale->X;
	matrix->right.Z *= vecScale->X;

	matrix->up.X *= vecScale->Y;
	matrix->up.Y *= vecScale->Y;
	matrix->up.Z *= vecScale->Y;

	matrix->at.X *= vecScale->Z;
	matrix->at.Y *= vecScale->Z;
	matrix->at.Z *= vecScale->Z;

	matrix->flags &= 0xFFFDFFFC;
}

bool IsPointInRect(float x, float y, RECT* rect)
{
	if (x >= rect->fLeft && x <= rect->fRight &&
		y >= rect->fBottom && y <= rect->fTop)
		return true;

	return false;
}

int GetFreeTextDrawTextureSlot()
{
	for (int i = 0; i < 200; i++)
	{
		if (bTextDrawTextureSlotState[i] == false) {
			bTextDrawTextureSlotState[i] = true;
			return i;
		}
	}

	return -1;
}

void DestroyTextDrawTexture(int index)
{
	if(index >= 0 && index < 200)
	{
		bTextDrawTextureSlotState[index] = false;
		TextDrawTexture[index] = 0x0;
	}
}
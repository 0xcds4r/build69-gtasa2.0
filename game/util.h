#pragma once

#include "common.h"

PED_TYPE* GamePool_FindPlayerPed();
PED_TYPE* GamePool_Ped_GetAt(int iID);
int GamePool_Ped_GetIndex(PED_TYPE *pActor);

VEHICLE_TYPE *GamePool_Vehicle_GetAt(int iID);
int GamePool_Vehicle_GetIndex(VEHICLE_TYPE *pVehicle);

ENTITY_TYPE *GamePool_Object_GetAt(int iID);

int LineOfSight(VECTOR* start, VECTOR* end, void* colpoint, uintptr_t ent,
	char buildings, char vehicles, char peds, char objects, char dummies, bool seeThrough, bool camera, bool unk);

bool IsPedModel(unsigned int uiModel);
bool IsValidModel(unsigned int uiModelID);
uint16_t GetModelReferenceCount(int nModelIndex);

void InitPlayerPedPtrRecords();
void SetPlayerPedPtrRecord(uint8_t bytePlayer, uintptr_t dwPedPtr);
uint8_t FindPlayerNumFromPedPtr(uintptr_t dwPedPtr);

uintptr_t GetTexture(const char* texture);
uintptr_t LoadTextureFromDB(const char* dbname, const char* texture);
void DefinedState2d();
void SetScissorRect(void* pRect);
float DegToRad(float fDegrees);
// 0.3.7
float FloatOffset(float f1, float f2);

// ~ added stuff
uintptr_t GetModelInfoByID(int iModelID);
uintptr_t ModelInfoCreateInstance(int iModel);
void RenderClumpOrAtomic(uintptr_t rwObject);
float GetModelColSphereRadius(int iModel);
void GetModelColSphereVecCenter(int iModel, VECTOR* vec);
void DestroyAtomicOrClump(uintptr_t rwObject);
void DrawTextureUV(uintptr_t texture, RECT* rect, uint32_t dwColor, float *uv);

uintptr_t LoadTextureFromTxd(const char *txd, const char *texture);
uintptr_t LoadTexture(const char* texname);
void DeleteRwTexture(uintptr_t pRwTexture);

void calculateAimVector(VECTOR *vec1, VECTOR *vec2);

void ProjectMatrix(VECTOR* vecOut, MATRIX4X4* mat, VECTOR *vecPos);
void RwMatrixOrthoNormalize(MATRIX4X4 *matIn, MATRIX4X4 *matOut);
void RwMatrixInvert(MATRIX4X4 *matOut, MATRIX4X4 *matIn);
void RwMatrixRotate(MATRIX4X4 *mat, int axis, float angle);
void RwMatrixScale(MATRIX4X4 *matrix, VECTOR *vecScale);

bool IsPointInRect(float x, float y, RECT* rect);
int GetFreeTextDrawTextureSlot();
void DestroyTextDrawTexture(int index);
#include "../main.h"
#include "game.h"
#include "RW/RenderWare.h"

extern CGame *pGame;

CSnapShotHelper::CSnapShotHelper()
{
	m_camera = 0;
	m_light = 0;
	m_frame = 0;
	m_zBuffer = 0;
	m_raster = 0;

	SetUpScene();
}

// 2.0
void CSnapShotHelper::SetUpScene()
{
	// RpLightCreate(int)	.text	00216E30	00000086	00000010	00000000	R	.	.	.	B	T	.
	m_light = ((uintptr_t(*)(int))(g_GTASAAdr + 0x216E30 + 1))(2);
	if (m_light == 0) return;

	// RpLightSetColor(RpLight *,RwRGBAReal const*)	.text	002167C6	00000038			R	.	.	.	.	.	.
	float rwColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	((void (*)(uintptr_t, float*))(g_GTASAAdr + 0x2167C6 + 1))(m_light, rwColor);

	// RwCameraCreate(void)	.text	001D5F60	000000FE	00000018	00000000	R	.	.	.	B	T	.
	m_camera = ((uintptr_t(*)())(g_GTASAAdr + 0x1D5F60 + 1))();

	// RwFrameCreate(void)	.text	001D822C	00000088	00000010	00000000	R	.	.	.	B	T	.
	m_frame = ((uintptr_t(*)())(g_GTASAAdr + 0x1D822C + 1))();

	// RwFrameTranslate(RwFrame *,RwV3d const*,RwOpCombineType)	.text	001D8694	00000056	00000010	00000000	R	.	.	.	B	.	.
	float v[3] = { 0.0f, 0.0f, 50.0f };
	((void(*)(uintptr_t, float*, int))(g_GTASAAdr + 0x1D8694 + 1))(m_frame, v, 1);

	// RwFrameRotate(RwFrame *,RwV3d const*,float,RwOpCombineType)	.text	001D87A8	00000056	00000010	00000000	R	.	.	.	B	.	.
	v[0] = 1.0f; v[1] = 0.0f; v[2] = 0.0f;
	((void(*)(uintptr_t, float*, float, int))(g_GTASAAdr + 0x1D87A8 + 1))(m_frame, v, 90.0f, 1);

	m_zBuffer = (uintptr_t)RwRasterCreate(256, 256, 0, rwRASTERTYPEZBUFFER);

	*(uintptr_t*)(m_camera + 0x64) = m_zBuffer;

	// _rwObjectHasFrameSetFrame(void *,RwFrame *)	.text	001DCFE4	00000032			R	.	.	.	.	.	.
	((void(*)(uintptr_t, uintptr_t))(g_GTASAAdr + 0x1DCFE4 + 1))(m_camera, m_frame);

	// RwCameraSetFarClipPlane(RwCamera *,float)	.text	001D5B4C	00000090	00000018	00000000	R	.	.	.	B	.	.
	((void(*)(uintptr_t, float))(g_GTASAAdr + 0x1D5B4C + 1))(m_camera, 300.0f);

	// RwCameraSetNearClipPlane(RwCamera *,float)	.text	001D5AB8	00000090	00000018	00000000	R	.	.	.	B	.	.
	((void(*)(uintptr_t, float))(g_GTASAAdr + 0x1D5AB8 + 1))(m_camera, 0.01f);

	// RwCameraSetViewWindow(RwCamera *,RwV2d const*)	.text	001D5E84	0000005C	00000010	00000000	R	.	.	.	B	.	.
	float view[2] = { 0.5f, 0.5f };
	((void(*)(uintptr_t, float*))(g_GTASAAdr + 0x1D5E84 + 1))(m_camera, view);

	// RwCameraSetProjection(RwCamera *,RwCameraProjection)	.text	001D5DA8	000000B2	00000020	00000000	R	.	.	.	B	.	.
	((void(*)(uintptr_t, int))(g_GTASAAdr + 0x1D5DA8 + 1))(m_camera, 1);

	// RpWorldAddCamera(RpWorld *,RwCamera *)	.text	0021E004	0000001E	00000010	00000000	R	.	.	.	B	.	.
	uintptr_t pRwWorld = *(uintptr_t*)(g_GTASAAdr + 0x9FC938);
	if (pRwWorld) {
		((void(*)(uintptr_t, uintptr_t))(g_GTASAAdr + 0x21E004 + 1))(pRwWorld, m_camera);
	}
}

// 2.0
uintptr_t CSnapShotHelper::CreatePedSnapShot(int iModel, uint32_t dwColor, VECTOR* vecRot, float fZoom)
{
	uintptr_t raster = (uintptr_t)RwRasterCreate(256, 256, 32, rwRASTERFORMAT8888 | rwRASTERTYPECAMERATEXTURE);

	// RwTextureCreate(RwRaster *)	.text	001DB83C	00000056	00000010	00000000	R	.	.	.	B	.	.
	uintptr_t bufferTexture = ((uintptr_t(*)(uintptr_t))(g_GTASAAdr + 0x1DB83C + 1))(raster);

	CPlayerPed *pPed = new CPlayerPed(208, 0, 0.0f, 0.0f, 0.0f, 0.0f);

	if(!raster || !bufferTexture || !pPed) return 0;

	float posZ = iModel == 162 ? 50.15f : 50.05f;
	float posY = fZoom * -2.25f;
	pPed->TeleportTo(0.0f, posY, posZ);
	pPed->SetModelIndex(iModel);
	pPed->SetGravityProcessing(false);
	pPed->SetCollisionChecking(false);

	MATRIX4X4 mat;
	pPed->GetMatrix(&mat);

	if(vecRot->X != 0.0f) {
		RwMatrixRotate(&mat, 0, vecRot->X);
	}

	if(vecRot->Y != 0.0f) {
		RwMatrixRotate(&mat, 1, vecRot->Y);
	}

	if(vecRot->Z != 0.0f) {
		RwMatrixRotate(&mat, 2, vecRot->Z);
	}

	pPed->UpdateMatrix(mat);

	// set frame raster
	*(uintptr_t*)(m_camera + 0x60) = raster;

	// CVisibilityPlugins::SetRenderWareCamera(RwCamera *)	.text	005D61F8	00000106	00000018	00000000	R	.	.	.	B	.	.
	((void(*)(uintptr_t))(g_GTASAAdr + 0x5D61F8 + 1))(m_camera);

	// RwCameraClear(RwCamera *,RwRGBA *,int)	.text	001D5D70	0000001E	00000010	00000000	R	.	.	.	B	.	.
	((void(*)(uintptr_t, uint32_t*, int))(g_GTASAAdr + 0x1D5D70 + 1))(m_camera, &dwColor, 3);

	RwCameraBeginUpdate((RwCamera*)m_camera);

	// RpWorldAddLight(RpWorld *,RpLight *)	.text	0021E830	00000040	00000010	00000000	R	.	.	.	B	.	.
	uintptr_t pRwWorld = *(uintptr_t*)(g_GTASAAdr + 0x9FC938);
	if (pRwWorld) {
		((void(*)(uintptr_t, uintptr_t))(g_GTASAAdr + 0x21E830 + 1))(pRwWorld, m_light);
	}

	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)true);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)true);
	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODENASHADEMODE);
	RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)0);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODENACULLMODE);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)false);

	// DefinedState
	((void(*)(void))(g_GTASAAdr + 0x5D0BC1))();

	pPed->Add();
	
	pPed->ClumpUpdateAnimations(100.0f, 1); 
	pPed->Render();

	RwCameraEndUpdate((RwCamera*)m_camera);

	// RpWorldRemoveLight(RpWorld *,RpLight *)	.text	0021E874	00000072	00000020	00000000	R	.	.	.	B	.	.
	if (pRwWorld) {
		((void(*)(uintptr_t, uintptr_t))(g_GTASAAdr + 0x21E874 + 1))(pRwWorld, m_light);
	}

	pPed->Remove();
	delete pPed;

	return bufferTexture;
}

uintptr_t CSnapShotHelper::CreateVehicleSnapShot(int iModel, uint32_t dwColor, VECTOR* vecRot, float fZoom, int dwColor1, int dwColor2)
{
	uintptr_t raster = (uintptr_t)RwRasterCreate(256, 256, 32, rwRASTERFORMAT8888 | rwRASTERTYPECAMERATEXTURE);

	// RwTextureCreate(RwRaster *)	.text	001DB83C	00000056	00000010	00000000	R	.	.	.	B	.	.
	uintptr_t bufferTexture = ((uintptr_t(*)(uintptr_t))(g_GTASAAdr + 0x1DB83C + 1))(raster);

	if(iModel == 570)
	{
		iModel = 538;
	}
	else if(iModel == 569)
	{
		iModel = 537;
	}

	CVehicle *pVehicle = new CVehicle(iModel, 0.0f, 0.0f, 50.0f, 0.0f);

	if(!raster || !bufferTexture || !pVehicle) {
		return 0;
	}

	pVehicle->SetGravityProcessing(false);
	pVehicle->SetCollisionChecking(false);

	float fRadius = GetModelColSphereRadius(iModel);
	float posY = (-1.0 - (fRadius + fRadius)) * fZoom;

	if(pVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_BOAT)
	{
		posY = -5.5 - fRadius * 2.5;
	}

	pVehicle->TeleportTo(0.0f, posY, 50.0f);

	if(dwColor1 != 0xFFFF && dwColor2 != 0xFFFF) {
		pVehicle->SetColor(dwColor1, dwColor2);
	}

	MATRIX4X4 mat;
	pVehicle->GetMatrix(&mat);

	if(vecRot->X != 0.0f) {
		RwMatrixRotate(&mat, 0, vecRot->X);
	}

	if(vecRot->Y != 0.0f) {
		RwMatrixRotate(&mat, 1, vecRot->Y);
	}

	if(vecRot->Z != 0.0f) {
		RwMatrixRotate(&mat, 2, vecRot->Z);
	}

	pVehicle->UpdateMatrix(mat);

	// set frame raster
	*(uintptr_t*)(m_camera + 0x60) = raster;

	// CVisibilityPlugins::SetRenderWareCamera(RwCamera *)	.text	005D61F8	00000106	00000018	00000000	R	.	.	.	B	.	.
	((void(*)(uintptr_t))(g_GTASAAdr + 0x5D61F8 + 1))(m_camera);

	// RwCameraClear(RwCamera *,RwRGBA *,int)	.text	001D5D70	0000001E	00000010	00000000	R	.	.	.	B	.	.
	((void(*)(uintptr_t, uint32_t*, int))(g_GTASAAdr + 0x1D5D70 + 1))(m_camera, &dwColor, 3);

	RwCameraBeginUpdate((RwCamera*)m_camera);

	// RpWorldAddLight(RpWorld *,RpLight *)	.text	0021E830	00000040	00000010	00000000	R	.	.	.	B	.	.
	uintptr_t pRwWorld = *(uintptr_t*)(g_GTASAAdr + 0x9FC938);
	if (pRwWorld) {
		((void(*)(uintptr_t, uintptr_t))(g_GTASAAdr + 0x21E830 + 1))(pRwWorld, m_light);
	}

	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)true);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)true);
	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODENASHADEMODE);
	RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)0);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODENACULLMODE);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)false);

	// DefinedState
	((void(*)(void))(g_GTASAAdr + 0x5D0BC1))();

	pVehicle->Add();
	pVehicle->Render();

	RwCameraEndUpdate((RwCamera*)m_camera);

	// RpWorldRemoveLight(RpWorld *,RpLight *)	.text	0021E874	00000072	00000020	00000000	R	.	.	.	B	.	.
	if (pRwWorld) {
		((void(*)(uintptr_t, uintptr_t))(g_GTASAAdr + 0x21E874 + 1))(pRwWorld, m_light);
	}

	pVehicle->Remove();

	delete pVehicle;

	return bufferTexture;
}

uintptr_t CSnapShotHelper::CreateObjectSnapShot(int iModel, uint32_t dwColor, VECTOR* vecRot, float fZoom)
{
	if (iModel == 1373 || iModel == 3118 || iModel == 3552 || iModel == 3553) {
		iModel = 18631;
	}

	bool bNeedRemoveModel = false;
	if (!pGame->IsModelLoaded(iModel))
	{
		pGame->RequestModel(iModel);
		pGame->LoadRequestedModels();
		while (!pGame->IsModelLoaded(iModel)) sleep(1);
		bNeedRemoveModel = true;
	}

	uintptr_t pRwObject = ModelInfoCreateInstance(iModel);

	float fRadius = GetModelColSphereRadius(iModel);

	VECTOR vecCenter = { 0.0f, 0.0f, 0.0f };
	GetModelColSphereVecCenter(iModel, &vecCenter);

	uintptr_t parent = *(uintptr_t*)(pRwObject + 4);

	if (parent == 0) {
		return 0;
	}

	// RwFrameTranslate(RwFrame *,RwV3d const*,RwOpCombineType)	.text	001D8694	00000056	00000010	00000000	R	.	.	.	B	.	.
	float v[3] = {
		-vecCenter.X,
		(-0.1f - fRadius * 2.25f) * fZoom,
		50.0f - vecCenter.Z };
	((void(*)(uintptr_t, float*, int))(g_GTASAAdr + 0x1D8694 + 1))(parent, v, 1);

	// RwFrameRotate(RwFrame *,RwV3d const*,float,RwOpCombineType)	.text	001D87A8	00000056	00000010	00000000	R	.	.	.	B	.	.
	if (iModel == 18631)
		{
			// RwFrameRotate X
			v[0] = 0.0f;
			v[1] = 0.0f;
			v[2] = 1.0f;
			((void(*)(uintptr_t, float*, float, int))(g_GTASAAdr + 0x1D87A8 + 1))(parent, v, 180.0f, 1);
		}
	else
	{
		if (vecRot->X != 0.0f)
		{
			// RwFrameRotate X
			v[0] = 1.0f;
			v[1] = 0.0f;
			v[2] = 0.0f;
			((void(*)(uintptr_t, float*, float, int))(g_GTASAAdr + 0x1D87A8 + 1))(parent, v, vecRot->X, 1);
		}

		if (vecRot->Y != 0.0f)
		{
			// RwFrameRotate Y
			v[0] = 0.0f;
			v[1] = 1.0f;
			v[2] = 0.0f;
			((void(*)(uintptr_t, float*, float, int))(g_GTASAAdr + 0x1D87A8 + 1))(parent, v, vecRot->Y, 1);
		}

		if (vecRot->Z != 0.0f)
		{
			// RwFrameRotate Z
			v[0] = 0.0f;
			v[1] = 0.0f;
			v[2] = 1.0f;
			((void(*)(uintptr_t, float*, float, int))(g_GTASAAdr + 0x1D87A8 + 1))(parent, v, vecRot->Z, 1);
		}
	}

	uintptr_t raster = (uintptr_t)RwRasterCreate(256, 256, 32, rwRASTERFORMAT8888 | rwRASTERTYPECAMERATEXTURE);

	// RwTextureCreate(RwRaster *)	.text	001DB83C	00000056	00000010	00000000	R	.	.	.	B	.	.
	uintptr_t bufferTexture = ((uintptr_t(*)(uintptr_t))(g_GTASAAdr + 0x1DB83C + 1))(raster);
	*(uintptr_t*)(m_camera + 0x60) = raster;

	// CVisibilityPlugins::SetRenderWareCamera(RwCamera *)	.text	005D61F8	00000106	00000018	00000000	R	.	.	.	B	.	.
	((void(*)(uintptr_t))(g_GTASAAdr + 0x5D61F8 + 1))(m_camera);

	ProcessCamera(pRwObject, dwColor);

	DestroyAtomicOrClump(pRwObject);

	if (bNeedRemoveModel) {
		pGame->RemoveModel(iModel, false);
	}

	return (uintptr_t)bufferTexture;
}

void CSnapShotHelper::ProcessCamera(uintptr_t pRwObject, uint32_t dwColor)
{
	// RwCameraClear(RwCamera *,RwRGBA *,int)	.text	001D5D70	0000001E	00000010	00000000	R	.	.	.	B	.	.
	((void(*)(uintptr_t, uint32_t*, int))(g_GTASAAdr + 0x1D5D70 + 1))(m_camera, &dwColor, 3);

	RwCameraBeginUpdate((RwCamera*)m_camera);

	// RpWorldAddLight(RpWorld *,RpLight *)	.text	0021E830	00000040	00000010	00000000	R	.	.	.	B	.	.
	uintptr_t pRwWorld = *(uintptr_t*)(g_GTASAAdr + 0x9FC938);
	if (pRwWorld) {
		((void(*)(uintptr_t, uintptr_t))(g_GTASAAdr + 0x21E830 + 1))(pRwWorld, m_light);
	}

	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)true);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)true);
	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODENASHADEMODE);
	RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)0);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODENACULLMODE);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)false);

	// DefinedState
	((void(*)(void))(g_GTASAAdr + 0x5D0BC1))();

	RenderClumpOrAtomic(pRwObject);

	RwCameraEndUpdate((RwCamera*)m_camera);

	// RpWorldRemoveLight(RpWorld *,RpLight *)	.text	0021E874	00000072	00000020	00000000	R	.	.	.	B	.	.
	if (pRwWorld) {
		((void(*)(uintptr_t, uintptr_t))(g_GTASAAdr + 0x21E874 + 1))(pRwWorld, m_light);
	}
}
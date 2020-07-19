#include "../../main.h"
#include "RenderWare.h"

RsGlobalType* RsGlobal;

/* rwcore.h */
RwCamera* (*RwCameraBeginUpdate)(RwCamera* camera);
RwCamera* (*RwCameraEndUpdate)(RwCamera* camera);
RwCamera* (*RwCameraShowRaster)(RwCamera * camera, void *pDev, RwUInt32 flags);

RwRaster* 	(*RwRasterCreate)(RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 flags);
RwBool 		(*RwRasterDestroy)(RwRaster * raster);
RwRaster* 	(*RwRasterGetOffset)(RwRaster *raster, RwInt16 *xOffset, RwInt16 *yOffset);
RwInt32 	(*RwRasterGetNumLevels)(RwRaster * raster);
RwRaster* 	(*RwRasterSubRaster)(RwRaster * subRaster, RwRaster * raster, RwRect * rect);
RwRaster* 	(*RwRasterRenderFast)(RwRaster * raster, RwInt32 x, RwInt32 y);
RwRaster* 	(*RwRasterRender)(RwRaster * raster, RwInt32 x, RwInt32 y);
RwRaster* 	(*RwRasterRenderScaled)(RwRaster * raster, RwRect * rect);
RwRaster* 	(*RwRasterPushContext)(RwRaster * raster);
RwRaster* 	(*RwRasterPopContext)(void);
RwRaster* 	(*RwRasterGetCurrentContext)(void);
RwBool 		(*RwRasterClear)(RwInt32 pixelValue);
RwBool 		(*RwRasterClearRect)(RwRect * rpRect, RwInt32 pixelValue);
RwRaster* 	(*RwRasterShowRaster)(RwRaster * raster, void *dev, RwUInt32 flags);
RwUInt8* 	(*RwRasterLock)(RwRaster * raster, RwUInt8 level, RwInt32 lockMode);
RwRaster* 	(*RwRasterUnlock)(RwRaster * raster);
RwUInt8* 	(*RwRasterLockPalette)(RwRaster * raster, RwInt32 lockMode);
RwRaster* 	(*RwRasterUnlockPalette)(RwRaster * raster);
RwImage* 	(*RwImageCreate)(RwInt32 width, RwInt32 height, RwInt32 depth);
RwBool 		(*RwImageDestroy)(RwImage * image);
RwImage* 	(*RwImageAllocatePixels)(RwImage * image);
RwImage* 	(*RwImageFreePixels)(RwImage * image);
RwImage* 	(*RwImageCopy)(RwImage * destImage, const RwImage * sourceImage);
RwImage* 	(*RwImageResize)(RwImage * image, RwInt32 width, RwInt32 height);
RwImage* 	(*RwImageApplyMask)(RwImage * image, const RwImage * mask);
RwImage* 	(*RwImageMakeMask)(RwImage * image);
RwImage* 	(*RwImageReadMaskedImage)(const RwChar * imageName, const RwChar * maskname);
RwImage* 	(*RwImageRead)(const RwChar * imageName);
RwImage* 	(*RwImageWrite)(RwImage * image, const RwChar * imageName);
RwImage* 	(*RwImageSetFromRaster)(RwImage *image, RwRaster *raster);
RwRaster* 	(*RwRasterSetFromImage)(RwRaster *raster, RwImage *image);
RwRaster* 	(*RwRasterRead)(const RwChar *filename);
RwRaster* 	(*RwRasterReadMaskedRaster)(const RwChar *filename, const RwChar *maskname);
RwImage* 	(*RwImageFindRasterFormat)(RwImage *ipImage, RwInt32 nRasterType, RwInt32 *npWidth, RwInt32 *npHeight, RwInt32 *npDepth,RwInt32 *npFormat);

/* rwlpcore.h */
RwReal 		(*RwIm2DGetNearScreenZ)(void);
RwReal 		(*RwIm2DGetFarScreenZ)(void);
RwBool 		(*RwRenderStateGet)(RwRenderState state, void *value);
RwBool 		(*RwRenderStateSet)(RwRenderState state, void *value);
RwBool 		(*RwIm2DRenderLine)(RwIm2DVertex *vertices, RwInt32 numVertices, RwInt32 vert1, RwInt32 vert2);
RwBool 		(*RwIm2DRenderTriangle)(RwIm2DVertex *vertices, RwInt32 numVertices, RwInt32 vert1, RwInt32 vert2, RwInt32 vert3 );
RwBool 		(*RwIm2DRenderPrimitive)(RwPrimitiveType primType, RwIm2DVertex *vertices, RwInt32 numVertices);
RwBool 		(*RwIm2DRenderIndexedPrimitive)(RwPrimitiveType primType, RwIm2DVertex *vertices, RwInt32 numVertices, RwImVertexIndex *indices, RwInt32 numIndices);

/* rtpng.h */
RwImage*	(*RtPNGImageWrite)(RwImage* image, const RwChar* imageName);
RwImage* 	(*RtPNGImageRead)(const RwChar* imageName);

void InitRenderWareFunctions()
{
	FLog("Initializing RenderWare..");

	/* skeleton.h */
	RsGlobal = (RsGlobalType*)(g_GTASAAdr+0x9FC8FC);

	/* rwCore.h */
	*(void**)(&RwCameraBeginUpdate) 			= (void*)(g_GTASAAdr+0x1D5A98+1);
	*(void**)(&RwCameraEndUpdate) 				= (void*)(g_GTASAAdr+0x1D5A94+1);
	*(void**)(&RwCameraShowRaster)				= (void*)(g_GTASAAdr+0x1D5D94+1);

	*(void **)(&RwRasterCreate) 				= (void*)(g_GTASAAdr+0x1DAA50+1);
	*(void **)(&RwRasterDestroy) 				= (void*)(g_GTASAAdr+0x1DA850+1);
	*(void **)(&RwRasterGetOffset) 				= (void*)(g_GTASAAdr+0x1DA72C+1);
	*(void **)(&RwRasterGetNumLevels) 			= (void*)(g_GTASAAdr+0x1DA980+1);
	*(void **)(&RwRasterSubRaster) 				= (void*)(g_GTASAAdr+0x1DA9F4+1);
	*(void **)(&RwRasterRenderFast)				= (void*)(g_GTASAAdr+0x1DA7B4+1);
	*(void **)(&RwRasterRender)					= (void*)(g_GTASAAdr+0x1DA800+1);
	*(void **)(&RwRasterRenderScaled)			= (void*)(g_GTASAAdr+0x1DA70C+1);
	*(void **)(&RwRasterPushContext)			= (void*)(g_GTASAAdr+0x1DA898+1);
	*(void **)(&RwRasterPopContext)				= (void*)(g_GTASAAdr+0x1DA938+1);
	*(void **)(&RwRasterGetCurrentContext)		= (void*)(g_GTASAAdr+0x1DA6EC+1);
	*(void **)(&RwRasterClear)					= (void*)(g_GTASAAdr+0x1DA75C+1);
	*(void **)(&RwRasterClearRect)				= (void*)(g_GTASAAdr+0x1DA7E0+1);
	*(void **)(&RwRasterShowRaster)				= (void*)(g_GTASAAdr+0x1DA9BC+1);
	*(void **)(&RwRasterLock)					= (void*)(g_GTASAAdr+0x1DAB24+1);
	*(void **)(&RwRasterUnlock)					= (void*)(g_GTASAAdr+0x1DA738+1);
	*(void **)(&RwRasterLockPalette)			= (void*)(g_GTASAAdr+0x1DA90C+1);
	*(void **)(&RwRasterUnlockPalette)			= (void*)(g_GTASAAdr+0x1DA82C+1);
	*(void **)(&RwImageCreate)					= (void*)(g_GTASAAdr+0x1D8EA0+1);
	*(void **)(&RwImageDestroy)					= (void*)(g_GTASAAdr+0x1D8EF8+1);
	*(void **)(&RwImageAllocatePixels)			= (void*)(g_GTASAAdr+0x1D8F84+1);
	*(void **)(&RwImageFreePixels)				= (void*)(g_GTASAAdr+0x1D8F58+1);
	*(void **)(&RwImageCopy)					= (void*)(g_GTASAAdr+0x1D95E0+1);
	*(void **)(&RwImageResize)					= (void*)(g_GTASAAdr+0x1D9020+1);
	*(void **)(&RwImageApplyMask)				= (void*)(g_GTASAAdr+0x1D9300+1);
	*(void **)(&RwImageMakeMask)				= (void*)(g_GTASAAdr+0x1D9148+1);
	*(void **)(&RwImageReadMaskedImage)			= (void*)(g_GTASAAdr+0x1D9E5C+1);
	*(void **)(&RwImageRead)					= (void*)(g_GTASAAdr+0x1D982C+1);
	*(void **)(&RwImageWrite)					= (void*)(g_GTASAAdr+0x1D9DC0+1);
	*(void **)(&RwImageSetFromRaster)			= (void*)(g_GTASAAdr+0x1DA4D4+1);
	*(void **)(&RwRasterSetFromImage)			= (void*)(g_GTASAAdr+0x1DA4F8+1);
	*(void **)(&RwRasterRead)					= (void*)(g_GTASAAdr+0x1DA5F4+1);
	*(void **)(&RwRasterReadMaskedRaster)		= (void*)(g_GTASAAdr+0x1DA694+1);
	*(void **)(&RwImageFindRasterFormat)		= (void*)(g_GTASAAdr+0x1DA51C+1);

	/* rwlpcore.h */
	*(void **)(&RwIm2DGetNearScreenZ)			= (void*)(g_GTASAAdr+0x1E28F4+1);
	*(void **)(&RwIm2DGetFarScreenZ)			= (void*)(g_GTASAAdr+0x1E2904+1);
	*(void **)(&RwRenderStateGet)				= (void*)(g_GTASAAdr+0x1E2948+1);
	*(void **)(&RwRenderStateSet)				= (void*)(g_GTASAAdr+0x1E2914+1);
	*(void **)(&RwIm2DRenderLine)				= (void*)(g_GTASAAdr+0x1E2958+1);
	*(void **)(&RwIm2DRenderTriangle)			= (void*)(g_GTASAAdr+0x1E2970+1);
	*(void **)(&RwIm2DRenderPrimitive)			= (void*)(g_GTASAAdr+0x1E2988+1);
	*(void **)(&RwIm2DRenderIndexedPrimitive)	= (void*)(g_GTASAAdr+0x1E2998+1);

	/* rtpng.h */
	*(void **)(&RtPNGImageWrite)				= (void*)(g_GTASAAdr+0x20A1C4+1);
	*(void **)(&RtPNGImageRead)					= (void*)(g_GTASAAdr+0x20A474+1);
}
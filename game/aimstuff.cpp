#include "main.h"
#include "game.h"

CAMERA_AIM* pcaInternalAim = 0;

CAMERA_AIM* GameGetInternalAim()
{
	return pcaInternalAim;
}

void GameAimSyncInit()
{
	pcaInternalAim = (CAMERA_AIM*)(g_GTASAAdr+0x952280);
}
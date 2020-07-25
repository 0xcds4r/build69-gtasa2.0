#include "main.h"
#include "game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "playertags.h"
#include "settings.h"

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CGUI *pGUI;
extern CSettings *pSettings;

CPlayerTags::CPlayerTags()
{
	FLog("Loading afk_icon..");
	// m_pAfk_icon = (RwTexture*)LoadTextureFromDB("samp", "afk_icon");

	HealthBarBDRColor = ImColor( 0x00, 0x00, 0x00, 0xFF );
}

void CPlayerTags::Render()
{
	if(!pNetGame) {
		return;
	}

	if(!pNetGame->m_bShowPlayerTags) {
		return;
	}

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();

	if(!pPlayerPool || !pVehiclePool) {
		return;
	}

	CLocalPlayer *pLocalPlayer = pPlayerPool->GetLocalPlayer();

	if(!pLocalPlayer) {
		return;
	}

	CPlayerPed *pLocalPed = pLocalPlayer->GetPlayerPed();

	if(!pLocalPed) {
		return;
	}

	MATRIX4X4 matLocal, matPlayer;

	pLocalPed->GetMatrix(&matLocal);
	if(!pLocalPed->IsValidMatrix(matLocal)) {
		return;
	}

	for(uint16_t playerId = 0; playerId < MAX_PLAYERS; playerId++)
	{
		if(pPlayerPool->GetSlotState(playerId) != true) {
			continue;
		}

		CRemotePlayer* pPlayer = pPlayerPool->GetAt(playerId);

		if(!pPlayer) {
			continue;
		}

		if(!pPlayer->IsActive() || !pPlayer->m_bShowNameTag) {
			continue;
		}

		CPlayerPed* pPlayerPed = pPlayer->GetPlayerPed();

		if(!pPlayerPed) {
			continue;
		}

		CAMERA_AIM *pCam = GameGetInternalAim();
		if(!pCam) {
			continue;
		}

		if(pPlayerPed->GetDistanceFromCamera() <= pNetGame->m_fNameTagDrawDistance)
		{
			if(!pPlayerPed->IsAdded()) {
				continue;
			}

			if(!pPlayerPed->GetRWObject()) {
				continue;
			}

			VECTOR VecPos;
			VecPos.X = 0.0f;
			VecPos.Y = 0.0f;
			VecPos.Z = 0.0f;
			pPlayerPed->GetBonePosition(8, &VecPos);

			if(!pPlayerPed->IsValidVector(VecPos)) {
				continue;
			}

			int dwHitEntity = 0;

			if(pNetGame->m_bNameTagLOS) {
				dwHitEntity = ScriptCommand(&get_line_of_sight, VecPos.X, VecPos.Y, VecPos.Z, pCam->pos1x, pCam->pos1y, pCam->pos1z, 1, 0, 0, 1, 0);
			}

			if(!pNetGame->m_bNameTagLOS || dwHitEntity)
			{
				char szNickBuf[0xF7];
				sprintf(szNickBuf, "%s (%d)", pPlayerPool->GetPlayerName(playerId), playerId);
				Draw(&VecPos, szNickBuf, pPlayer->GetPlayerColor(), pPlayerPed->GetDistanceFromCamera(), pPlayer->m_fReportedHealth, pPlayer->m_fReportedArmour, pPlayer->IsAFK());
			}
		}
	}
}

void CPlayerTags::Draw(VECTOR* vec, char* szName, uint32_t dwColor, float fDist, float fHealth, float fArmour, bool bAfk)
{
	VECTOR TagPos;

	TagPos.X = vec->X;
	TagPos.Y = vec->Y;
	TagPos.Z = vec->Z;
	TagPos.Z += 0.25f + (fDist * 0.0475f);

	VECTOR Out;

	// CSprite::CalcScreenCoors
	(( void (*)(VECTOR*, VECTOR*, float*, float*, bool, bool))(g_GTASAAdr+0x5C5798+1))(&TagPos, &Out, 0, 0, 0, 0);

	if(Out.Z < 1.0f) {
		return;
	}

	ImVec2 pos = ImVec2(Out.X, Out.Y);
	pos.x -= ImGui::CalcTextSize(szName).x / 2;
	pGUI->RenderBGText(pGUI->GetFontSize(), pos, __builtin_bswap32(dwColor | (0x000000FF)), true, szName);

	if(fHealth < 0.0f) {
		return;
	}

	Out.X = (float)((int)Out.X);
	Out.Y = (float)((int)Out.Y);

	HealthBarColor = ImColor( 0xB9, 0x22, 0x28, 0xFF );
	HealthBarBGColor = ImColor( 0x4B, 0x0B, 0x14, 0xFF );

	float fWidth = pGUI->ScaleX( 100.0f );
	float fHeight = pGUI->ScaleY( 13.75f );
	float fOutline = 2.0f;

	// top left
	HealthBarBDR1.x = Out.X - ((fWidth/2) + fOutline);
	HealthBarBDR1.y = Out.Y + (pGUI->GetFontSize() * 1.2f);

	// bottom right
	HealthBarBDR2.x = Out.X + ((fWidth/2) + fOutline);
	HealthBarBDR2.y = Out.Y + (pGUI->GetFontSize() * 1.2f) + fHeight;

	// top left
	HealthBarBG1.x = HealthBarBDR1.x + fOutline;
	HealthBarBG1.y = HealthBarBDR1.y + fOutline;

	// bottom right
	HealthBarBG2.x = HealthBarBDR2.x - fOutline;
	HealthBarBG2.y = HealthBarBDR2.y - fOutline;

	// top left
	HealthBar1.x = HealthBarBG1.x;
	HealthBar1.y = HealthBarBG1.y;

	// bottom right
	HealthBar2.y = HealthBarBG2.y;

	if (fHealth >= 100.0f) {
		fHealth = 100.0f;
	}

	fHealth *= fWidth/100.0f;
	fHealth -= (fWidth/2);
	HealthBar2.x = Out.X + fHealth;

	if(fArmour > 0.0f)
	{
		HealthBarBDR1.y += 13.0f;
		HealthBarBDR2.y += 13.0f;
		HealthBarBG1.y += 13.0f;
		HealthBarBG2.y += 13.0f;
		HealthBar1.y += 13.0f;
		HealthBar2.y += 13.0f;
	}

	ImGui::GetBackgroundDrawList()->AddRectFilled(HealthBarBDR1, HealthBarBDR2, HealthBarBDRColor);
	ImGui::GetBackgroundDrawList()->AddRectFilled(HealthBarBG1, HealthBarBG2, HealthBarBGColor);
	ImGui::GetBackgroundDrawList()->AddRectFilled(HealthBar1, HealthBar2, HealthBarColor);

	// Armour Bar
	if(fArmour > 0.0f)
	{
		HealthBarBDR1.y -= 13.0f;
		HealthBarBDR2.y -= 13.0f;
		HealthBarBG1.y -= 13.0f;
		HealthBarBG2.y -= 13.0f;
		HealthBar1.y -= 13.0f;
		HealthBar2.y -= 13.0f;

		HealthBarColor = ImColor(200, 200, 200, 255);
		HealthBarBGColor = ImColor(40, 40, 40, 255);

		if(fArmour > 100.0f) {
			fArmour = 100.0f;
		}

		fArmour *= fWidth/100.0f;
		fArmour -= (fWidth/2);
		HealthBar2.x = Out.X + fArmour;

		ImGui::GetBackgroundDrawList()->AddRectFilled(HealthBarBDR1, HealthBarBDR2, HealthBarBDRColor);
		ImGui::GetBackgroundDrawList()->AddRectFilled(HealthBarBG1, HealthBarBG2, HealthBarBGColor);
		ImGui::GetBackgroundDrawList()->AddRectFilled(HealthBar1, HealthBar2, HealthBarColor);
	}

	// AFK Icon
	if(bAfk && m_pAfk_icon && m_pAfk_icon->raster)
	{
		ImVec2 a = ImVec2(HealthBarBDR1.x - (pGUI->GetFontSize()*1.4f), HealthBarBDR1.y);
		ImVec2 b = ImVec2(a.x + (pGUI->GetFontSize()*1.3f), a.y + (pGUI->GetFontSize()*1.3f));

		stRect rect;
		stfRect uv;
		stfRect sRect;

		rect.x1 = a.x;
		rect.y1 = a.y;
		rect.x2 = b.x;
		rect.y2 = b.y;
		uv.x1 = 0.0f;
		uv.y1 = 0.0f;
		uv.x2 = 1.0;
		uv.y2 = 1.0;
		// DrawRaster(&rect, COLOR_WHITE, m_pAfk_icon->raster, &uv);
	}
}
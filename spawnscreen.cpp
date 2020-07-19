#include "main.h"
#include "gui/gui.h"
#include "spawnscreen.h"
#include "chatwindow.h"
#include "settings.h"

#include "game/game.h"
#include "net/netgame.h"

extern CGUI *pGUI;
extern CNetGame *pNetGame;
extern CChatWindow *pChatWindow;
extern CSettings *pSettings;

CSpawnScreen::CSpawnScreen()
{
	FLog("Initializing Spawn Screen..");
	m_fPosX = pGUI->ScaleX( 500.0f );
	m_fPosY = pGUI->ScaleY( 600.0f );
	m_fSizeX = pGUI->ScaleX( 700.0f );
	m_fSizeY = pGUI->ScaleY( 100.0f );
	m_fButWidth = m_fSizeX / 3;
	m_fButHeight = m_fSizeY * 0.9;
	FLog("Spawn Screen pos: %f, %f, size: %f, %f", m_fPosX, m_fPosY, m_fSizeX, m_fSizeY);

	m_bEnabled = false;
}

void CSpawnScreen::Render()
{
	if(!m_bEnabled) return;

	ImGuiIO &io = ImGui::GetIO();
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	CLocalPlayer *pLocalPlayer = 0;
	if(pPlayerPool) pLocalPlayer = pPlayerPool->GetLocalPlayer();

	ImGui::Begin("SpawnScreen", nullptr, 
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | 
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings);

	if (ImGui::Button("<<", ImVec2(m_fButWidth, m_fButHeight)))
	{
		pLocalPlayer->SendPrevClass();
	}
	ImGui::SameLine(0, 0);
	if (ImGui::Button("Spawn", ImVec2(m_fButWidth, m_fButHeight)))
	{
		pLocalPlayer->SendSpawn();
	}
	ImGui::SameLine(0, 0);
	if (ImGui::Button(">>", ImVec2(m_fButWidth, m_fButHeight)))
	{
		pLocalPlayer->SendNextClass();
	}

	ImGui::SetWindowSize(ImVec2(-1, -1));
	ImVec2 size = ImGui::GetWindowSize();
	
	ImGui::SetWindowPos( ImVec2( ((io.DisplaySize.x - size.x)/2), ((io.DisplaySize.y * 0.95) - size.y) ) );
	ImGui::End();
}
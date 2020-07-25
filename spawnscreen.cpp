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
	m_fButHeight = m_fSizeY * 0.65;
	FLog("Spawn Screen pos: %f, %f, size: %f, %f", m_fPosX, m_fPosY, m_fSizeX, m_fSizeY);

	m_bEnabled = false;
}

void CSpawnScreen::PresetsApply()
{
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	style->WindowPadding = ImVec2(13, 4);
	style->WindowRounding = 0.0f;
	style->FramePadding = ImVec2(6, 3);
	style->FrameRounding = 20.0f;
	style->ItemSpacing = ImVec2(15, 4);
	style->ItemInnerSpacing = ImVec2(14, 10);
	style->IndentSpacing = 21.0f;
	// style->ScrollbarSize = 16.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 18.0f;
	style->GrabRounding = 8.0f;

	style->WindowBorderSize = 0.0f;
	style->ChildBorderSize  = 1.0f;
	style->PopupBorderSize  = 1.0f;
	style->FrameBorderSize  = 1.0f;
	style->Alpha            = 1.0f;

	colors[ImGuiCol_Text] 					= ImVec4(RGBA_TO_FLOAT(160, 194, 222, 0xFF));
	colors[ImGuiCol_TextDisabled] 			= ImVec4(RGBA_TO_FLOAT(128, 128, 128, 0xFF));
	colors[ImGuiCol_WindowBg]				= ImVec4(RGBA_TO_FLOAT(0, 0, 0, 245));
	colors[ImGuiCol_ChildBg] 				= ImVec4(RGBA_TO_FLOAT(0, 0, 0, 0xFF));
	colors[ImGuiCol_PopupBg] 				= ImVec4(RGBA_TO_FLOAT(20, 20, 20, 240));
	colors[ImGuiCol_Border] 				= ImVec4(RGBA_TO_FLOAT(255, 255, 255, 0xFF));
	colors[ImGuiCol_BorderShadow] 			= ImVec4(RGBA_TO_FLOAT(255, 255, 255, 0xFF));
	colors[ImGuiCol_FrameBg] 				= ImVec4(RGBA_TO_FLOAT(0, 0, 0, 0xFF));
	colors[ImGuiCol_FrameBgHovered] 		= ImVec4(RGBA_TO_FLOAT(40, 40, 40, 0xFF));
	colors[ImGuiCol_FrameBgActive] 			= ImVec4(RGBA_TO_FLOAT(40, 40, 40, 0xFF));
	colors[ImGuiCol_TitleBg] 				= ImVec4(RGBA_TO_FLOAT(10, 10, 10, 0xFF));
	colors[ImGuiCol_TitleBgCollapsed] 		= ImVec4(RGBA_TO_FLOAT(0, 0, 0, 130));
	colors[ImGuiCol_TitleBgActive] 			= ImVec4(RGBA_TO_FLOAT(16, 16, 16, 0xFF));
	colors[ImGuiCol_MenuBarBg] 				= ImVec4(RGBA_TO_FLOAT(36, 36, 36, 0xFF));
	colors[ImGuiCol_ScrollbarBg] 			= ImVec4(RGBA_TO_FLOAT(36, 36, 36, 135));
	colors[ImGuiCol_ScrollbarGrab] 			= ImVec4(RGBA_TO_FLOAT(177, 40, 0, 0xFF));
	colors[ImGuiCol_ScrollbarGrabHovered] 	= ImVec4(RGBA_TO_FLOAT(239, 16, 16, 0xFF));
	colors[ImGuiCol_ScrollbarGrabActive] 	= ImVec4(RGBA_TO_FLOAT(249, 0, 0, 0xFF));
	colors[ImGuiCol_CheckMark] 				= ImVec4(RGBA_TO_FLOAT(21, 21, 21, 0xFF));
	colors[ImGuiCol_SliderGrab] 			= ImVec4(RGBA_TO_FLOAT(149, 42, 42, 0xFF));
	colors[ImGuiCol_SliderGrabActive] 		= ImVec4(RGBA_TO_FLOAT(201, 55, 21, 0xFF));
	colors[ImGuiCol_Button] 				= ImVec4(RGBA_TO_FLOAT(0, 0, 0, 0xFF));
	colors[ImGuiCol_ButtonHovered] 			= ImVec4(RGBA_TO_FLOAT(19, 19, 19, 0xFF));
	colors[ImGuiCol_ButtonActive] 			= ImVec4(RGBA_TO_FLOAT(60, 60, 60, 0xFF));
	colors[ImGuiCol_Header] 				= ImVec4(RGBA_TO_FLOAT(166, 50, 0, 0xFF));
	colors[ImGuiCol_HeaderHovered] 			= ImVec4(RGBA_TO_FLOAT(54, 53, 52, 0xFF));
	colors[ImGuiCol_HeaderActive] 			= ImVec4(RGBA_TO_FLOAT(166, 50, 0, 0xFF));
	colors[ImGuiCol_ResizeGrip] 			= ImVec4(RGBA_TO_FLOAT(32, 32, 32, 0xFF));
	colors[ImGuiCol_ResizeGripHovered] 		= ImVec4(RGBA_TO_FLOAT(32, 32, 32, 0xFF));
	colors[ImGuiCol_ResizeGripActive] 		= ImVec4(RGBA_TO_FLOAT(32, 32, 32, 0xFF));
	colors[ImGuiCol_PlotLines] 				= ImVec4(RGBA_TO_FLOAT(32, 32, 32, 0xFF));
	colors[ImGuiCol_PlotLinesHovered] 		= ImVec4(RGBA_TO_FLOAT(32, 32, 32, 0xFF));
	colors[ImGuiCol_PlotHistogram] 			= ImVec4(RGBA_TO_FLOAT(32, 32, 32, 0xFF));
	colors[ImGuiCol_PlotHistogramHovered] 	= ImVec4(RGBA_TO_FLOAT(32, 32, 32, 0xFF));
	colors[ImGuiCol_TextSelectedBg] 		= ImVec4(RGBA_TO_FLOAT(255, 220, 255, 0xFF));
	colors[ImGuiCol_ModalWindowDarkening] 	= ImVec4(RGBA_TO_FLOAT(32, 32, 32, 0xFF));
}

void CSpawnScreen::Render()
{
	if(!m_bEnabled) return;
	PresetsApply();

	ImGuiIO &io = ImGui::GetIO();
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	CLocalPlayer *pLocalPlayer = 0;
	if(pPlayerPool) pLocalPlayer = pPlayerPool->GetLocalPlayer();

	ImGui::SetNextWindowBgAlpha(0.0f);

	ImGui::Begin("SpawnScreen", nullptr, 
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | 
		ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings);

	if (ImGui::Button("<<", ImVec2(m_fButWidth, m_fButHeight)))
	{
		pLocalPlayer->SendPrevClass();
		// PresetsApplyWhenTouch();
	}
	PresetsApply();

	ImGui::SameLine(0, pGUI->GetFontSize());
	if (ImGui::Button(">>", ImVec2(m_fButWidth, m_fButHeight)))
	{
		pLocalPlayer->SendNextClass();
		// PresetsApplyWhenTouch();
	}
	
	PresetsApply();

	ImGui::SameLine(0, pGUI->GetFontSize());
	if (ImGui::Button("Spawn", ImVec2(m_fButWidth, m_fButHeight)))
	{
		pLocalPlayer->SendSpawn();
		// PresetsApplyWhenTouch();
	}
	PresetsApply();

	ImGui::SetWindowSize(ImVec2(-1, -1));
	ImVec2 size = ImGui::GetWindowSize();
	
	ImGui::SetWindowPos( ImVec2( ((io.DisplaySize.x - size.x)/2), ((io.DisplaySize.y * 0.95) - size.y) ) );
	ImGui::End();
}
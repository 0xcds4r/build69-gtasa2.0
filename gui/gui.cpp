#include "main.h"
#include "gui.h"
#include "game/game.h"
#include "net/netgame.h"
#include "game/RW/RenderWare.h"
#include "chatwindow.h"
#include "spawnscreen.h"
#include "playertags.h"
#include "dialog.h"
#include "keyboard.h"
#include "debug.h"
#include "settings.h"

extern CChatWindow *pChatWindow;
extern CSpawnScreen *pSpawnScreen;
extern CPlayerTags *pPlayerTags;
extern CDialogWindow *pDialogWindow;
extern CDebug *pDebug;
extern CSettings *pSettings;
extern CKeyBoard *pKeyBoard;
extern CNetGame *pNetGame;
extern CGame *pGame;

/* imgui_impl_renderware.h */
void ImGui_ImplRenderWare_RenderDrawData(ImDrawData* draw_data);
bool ImGui_ImplRenderWare_Init();
void ImGui_ImplRenderWare_NewFrame();
void ImGui_ImplRenderWare_ShutDown();

/*
	Все координаты GUI-элементов задаются
	относительно разрешения 1920x1080
*/
#define MULT_X	0.00052083333f	// 1/1920
#define MULT_Y	0.00092592592f 	// 1/1080

CGUI::CGUI()
{
	FLog("Initializing GUI..");

	// setup ImGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();

	ImGui_ImplRenderWare_Init();

	// scale
	m_vecScale.x = io.DisplaySize.x * MULT_X;
	m_vecScale.y = io.DisplaySize.y * MULT_Y;

	// font Size
	m_fFontSize = ScaleY( 30.0f );

	// mouse/touch
	m_bMousePressed = false;
	m_vecMousePos = ImVec2(0, 0);

	FLog("GUI | Scale factor: %f, %f Font size: %f", m_vecScale.x, m_vecScale.y, m_fFontSize);

	// setup style
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScrollbarSize = ScaleY(55.0f);
	style.WindowBorderSize = 0.0f;
	ImGui::StyleColorsDark();

	// load fonts
	char path[0xFF] = { 0 };
	sprintf(path, "%sfonts/arial_bold.ttf", (const char*)(g_GTASAAdr+0x6D687C));

	// cp1251 ranges
	static const ImWchar ranges[] = 
	{
		0x0020, 0x0080,
		0x00A0, 0x00C0,
		0x0400, 0x0460,
		0x0490, 0x04A0,
		0x2010, 0x2040,
		0x20A0, 0x20B0,
		0x2110, 0x2130,
		0
	};
	
	FLog("GUI | Loading font: arial_bold.ttf");
	m_pFont = io.Fonts->AddFontFromFileTTF(path, m_fFontSize, nullptr, ranges);
	FLog("GUI | ImFont pointer = 0x%X", m_pFont);
}

CGUI::~CGUI()
{
	ImGui_ImplRenderWare_ShutDown();
	ImGui::DestroyContext();
}

void CGUI::PresetsApply()
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
	style->ScrollbarSize = ScaleY(45.0f);
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
	colors[ImGuiCol_FrameBgHovered] 		= ImVec4(RGBA_TO_FLOAT(40, 40, 40, 120));
	colors[ImGuiCol_FrameBgActive] 			= ImVec4(RGBA_TO_FLOAT(40, 40, 40, 140));
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
	colors[ImGuiCol_Button] 				= ImVec4(RGBA_TO_FLOAT(0, 0, 0, 102));
	colors[ImGuiCol_ButtonHovered] 			= ImVec4(RGBA_TO_FLOAT(19, 19, 19, 0xFF));
	colors[ImGuiCol_ButtonActive] 			= ImVec4(RGBA_TO_FLOAT(60, 60, 60, 0xFF));
	colors[ImGuiCol_Header] 				= ImVec4(RGBA_TO_FLOAT(166, 50, 0, 79));
	colors[ImGuiCol_HeaderHovered] 			= ImVec4(RGBA_TO_FLOAT(54, 53, 52, 79));
	colors[ImGuiCol_HeaderActive] 			= ImVec4(RGBA_TO_FLOAT(166, 50, 0, 79));
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

void CGUI::Render()
{
	if(pGame)
	{
		if(pGame->IsGameLoaded())
		{
			if(pGame->IsGamePaused())
			{
				return;
			}
		}
	}

	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplRenderWare_NewFrame();
	ImGui::NewFrame();
	PresetsApply();

	if(pPlayerTags) {
		pPlayerTags->Render();
	}
	
	if(pChatWindow) {
		pChatWindow->Render();
	}

	if(pSpawnScreen) {
		pSpawnScreen->Render();
	}

	if(pDialogWindow) {
		pDialogWindow->Render();
	}

	if(pKeyBoard) {
		pKeyBoard->Render();
	}

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplRenderWare_RenderDrawData(ImGui::GetDrawData());

	if(m_bNeedClearMousePos)
	{
		io.MousePos = ImVec2(-1, -1);
		m_bNeedClearMousePos = false;
	}
}

bool CGUI::OnTouchEvent(int type, bool multi, int x, int y)
{
	ImGuiIO& io = ImGui::GetIO();

	if(pGame)
	{
		if(pGame->IsGameLoaded())
		{
			if(pGame->IsGamePaused())
			{
				return true;
			}
		}
	}

	if(pKeyBoard)
	{
		if(!pKeyBoard->OnTouchEvent(type, multi, x, y)) 
		{
			return false;
		}
	}

	if(pChatWindow)
	{
		if(!pChatWindow->OnTouchEvent(type, multi, x, y)) {
			return false;
		}
	}

	switch(type)
	{
		case TOUCH_PUSH:
		io.MousePos = ImVec2(x, y);
		io.MouseDown[0] = true;
		break;

		case TOUCH_POP:
		io.MouseDown[0] = false;
		m_bNeedClearMousePos = true;
		break;

		case TOUCH_MOVE:
		io.MousePos = ImVec2(x, y);
		break;
	}

	return true;
}

void CGUI::RenderVersion()
{
	// nothing
}

void CGUI::RenderRakNetStatistics()
{
	// nothing
}

void CGUI::RenderText(ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end)
{
	int iOffset = 2.0f;

	if(bOutline)
	{
		posCur.x -= iOffset;
		ImGui::GetOverlayDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.x += iOffset;
		// right 
		posCur.x += iOffset;
		ImGui::GetOverlayDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.x -= iOffset;
		// above
		posCur.y -= iOffset;
		ImGui::GetOverlayDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.y += iOffset;
		// below
		posCur.y += iOffset;
		ImGui::GetOverlayDrawList()->AddText(posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.y -= iOffset;
	}

	ImGui::GetOverlayDrawList()->AddText(posCur, col, text_begin, text_end);
}

void CGUI::RenderBGText(float fontSize, ImVec2& posCur, ImU32 col, bool bOutline, const char* text_begin, const char* text_end)
{
	int iOffset = 2;

	if(bOutline)
	{
		posCur.x -= iOffset;
		ImGui::GetBackgroundDrawList()->AddText(GetFont(), fontSize, posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.x += iOffset;
		// right 
		posCur.x += iOffset;
		ImGui::GetBackgroundDrawList()->AddText(GetFont(), fontSize, posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.x -= iOffset;
		// above
		posCur.y -= iOffset;
		ImGui::GetBackgroundDrawList()->AddText(GetFont(), fontSize, posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.y += iOffset;
		// below
		posCur.y += iOffset;
		ImGui::GetBackgroundDrawList()->AddText(GetFont(), fontSize, posCur, ImColor(IM_COL32_BLACK), text_begin, text_end);
		posCur.y -= iOffset;
	}

	ImGui::GetBackgroundDrawList()->AddText(GetFont(), fontSize, posCur, col, text_begin, text_end);
}
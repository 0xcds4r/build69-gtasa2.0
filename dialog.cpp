#include "main.h"
#include "game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "dialog.h"
#include "vendor/imgui/imgui_internal.h"
#include "keyboard.h"

extern CGUI *pGUI;
extern CGame *pGame;
extern CNetGame *pNetGame;
extern CKeyBoard *pKeyBoard;

char szDialogInputBuffer[100];
char utf8DialogInputBuffer[100*3];

CDialogWindow::CDialogWindow()
{
	m_bIsActive = false;
	m_putf8Info = nullptr;
	m_pszInfo = nullptr;
	m_selectedItem = 0;
}

void CDialogWindow::StyleColorsDialog(float roundSize)
{
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	style->FrameRounding = roundSize;
}

void CDialogWindow::Show(bool bShow)
{
	if(pGame) {
		pGame->FindPlayerPed()->TogglePlayerControllable(!bShow);
	}

	m_bIsActive = bShow;

	if (bShow) 
	{
		if (m_byteDialogStyle == DIALOG_STYLE_TABLIST_HEADERS)
		{
			m_iSelectedItem = 1;
		}
		else
		{
			m_iSelectedItem = 0;
		}
	}
}

void CDialogWindow::Clear()
{
	if(m_putf8Info)
	{
		free(m_putf8Info);
		m_putf8Info = nullptr;
	}

	if(m_pszInfo)
	{
		free(m_pszInfo);
		m_pszInfo = nullptr;
	}

	m_bIsActive = false;
	m_byteDialogStyle = 0;
	m_wDialogID = -1;
	m_utf8Title[0] = 0;
	m_utf8Button1[0] = 0;
	m_utf8Button2[0] = 0;

	m_fSizeX = -1.0f;
	m_fSizeY = -1.0f;

	memset(szDialogInputBuffer, 0, 100);
	memset(utf8DialogInputBuffer, 0, 100*3);
}

void CDialogWindow::SetInfo(char* szInfo, int length)
{
	if(!szInfo || !length) return;

	if(m_putf8Info)
	{
		free(m_putf8Info);
		m_putf8Info = nullptr;
	}

	if(m_pszInfo)
	{
		free(m_pszInfo);
		m_pszInfo = nullptr;
	}

	m_putf8Info = (char*)malloc((length * 3) + 1);
	if(!m_putf8Info)
	{
		FLog("CDialog::SetInfo: can't allocate memory");
		return;
	}

	m_pszInfo = (char*)malloc((length * 3) + 1);
	if(!m_pszInfo)
	{
		FLog("CDialog::SetInfo: can't allocate memory");
		return;
	}

	cp1251_to_utf8(m_putf8Info, szInfo);

	// =========
	char szNonColorEmbeddedMsg[4200];
	int iNonColorEmbeddedMsgLen = 0;

	for (size_t pos = 0; pos < strlen(szInfo) && szInfo[pos] != '\0'; pos++)
	{
		if(pos+7 < strlen(szInfo))
		{
			if (szInfo[pos] == '{' && szInfo[pos+7] == '}')
			{
				pos += 7;
				continue;
			}
		}

		szNonColorEmbeddedMsg[iNonColorEmbeddedMsgLen] = szInfo[pos];
		iNonColorEmbeddedMsgLen++;
	}

	szNonColorEmbeddedMsg[iNonColorEmbeddedMsgLen] = 0;
	// ========

	cp1251_to_utf8(m_pszInfo, szNonColorEmbeddedMsg);
}

void DialogWindowInputHandler(const char* str)
{
	if(!str || *str == '\0') return;
	strcpy(szDialogInputBuffer, str);
	cp1251_to_utf8(utf8DialogInputBuffer, str);
}

void CDialogWindow::PutListHeadersTitle()
{
	char bufString[4096];
    strcpy(bufString, m_putf8Info);
    char *str = bufString;
    char *pch;

    int i = 0;
    pch = strtok (str, "\n");

    if(i != 1 && pch != NULL)
    {
        char bufStr[16*5];
		if(i == 0) sprintf(bufStr, "%s", pch); 
			else return;

		TextWithColors(bufStr);
		pch = strtok(NULL, "\n");
    }
}

void CDialogWindow::MakeChildForList()
{
	if( (ImGui::CalcTextSize(m_pszInfo).x * 1.4f + pGUI->GetFontSize() * 3) > (ImGui::CalcTextSize("QWERTYUIOPASQWERTYUIOPASQWERTY").x) * 2)
		ImGui::BeginChild("list_graph_id", ImVec2(ImGui::CalcTextSize(m_pszInfo).x * 1.4f+pGUI->GetFontSize() * 2, 500), true, 0);
	else
		ImGui::BeginChild("list_graph_id2", ImVec2((ImGui::CalcTextSize("QWERTYUIOPASQWERTYUIOPASQWERTY").x) * 2 + ImGui::GetStyle().ScrollbarSize + pGUI->GetFontSize(), 500), true, 0);
}

void CDialogWindow::MakeList()
{
	ImVec2 vecButSize = ImVec2(ImGui::GetFontSize() * ImGui::CalcTextSize("QWERTYUIOPASDFGHJKLZXCVBNMQWERTYUASDGFHGDSFDSSDF").x, ImGui::GetFontSize() * 2.5);

	std::vector<std::string> lines;

	ImGuiContext& g = *GImGui;

	ImGui::GetStyle().WindowPadding = ImVec2(4, 10);

	ImGuiIO &io = ImGui::GetIO();

	MakeChildForList();

	std::string line = m_putf8Info; 

	while(line.find('\n') != -1)
	{
		unsigned short int poss = line.find('\n');
		if(poss <= 0) break;

		std::string buffer;
		buffer.assign(line, 0, poss);
		line.erase(0, poss + 1);

		if(buffer[0] != '\0') lines.push_back(buffer);

		if(poss <= 0) break;
	}

	if(line[0] != '\0') lines.push_back(line);

	ImGui::GetStyle().ButtonTextAlign = ImVec2(0.0f, 0.5f);
	ImGui::GetStyle().ItemSpacing = ImVec2(0.0f, 0.0f);

	unsigned int pSize = lines.size();

	for(int i = 0; i < pSize; i++)
	{	
		ImGuiStyle style;
		style.Colors[ImGuiCol_Button] = ImGui::GetStyle().Colors[ImGuiCol_Button];

		// first touch
		if(m_selectedItem == i) ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(RGBA_TO_FLOAT(155, 34, 34, 255));

		ImVec2 curpos = ImGui::GetCursorPos();
		std::string szidname;

		for(int x = 0; x < i; x++) szidname.insert(szidname.begin(), ' ');

		ImVec2 butSize;

		butSize.x = vecButSize.x;
		
		if( ImGui::Button((char*)szidname.c_str(), ImVec2(butSize.x, ImGui::GetFontSize() * 2.45f)) )
		{
			if(m_selectedItem == i)
			{ // double touch
				ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(0.0f, 0.0f, 0.0f, 0.95f); 
			
				if(m_wDialogID > 0)
				{
					if(pNetGame) 
					{
						pNetGame->SendDialogResponse(m_wDialogID, 1, i, (char*)lines[i].c_str());
						Show(false);
					}
				}

				return;
			}

			strcpy(szDialogInputBuffer, lines[i].c_str());
			m_selectedItem = i;
		}

		ImGui::ItemSize( ImVec2(0, 15) );

		ImVec2 newxtcurpos = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(curpos.x+pGUI->GetFontSize(), curpos.y+pGUI->GetFontSize()/2+3.0));

		TextWithColors(lines[i].c_str());
		ImGui::SetCursorPos(newxtcurpos);
		
		ImGui::GetStyle().Colors[ImGuiCol_Button] = style.Colors[ImGuiCol_Button];
	}

	ImGui::GetStyle().ButtonTextAlign = ImVec2(0.5f,0.5f);
	ImGui::EndChild();
	ImGui::Dummy(ImVec2(1, pGUI->GetFontSize()));
	ImGui::GetStyle().WindowPadding = ImVec2(pGUI->GetFontSize(), pGUI->GetFontSize());
}

void CDialogWindow::MakeListHeaders()
{
	ImVec2 vecButSize = ImVec2(ImGui::GetFontSize() * ImGui::CalcTextSize("QWERTYUIOPASDFGHJKLZXCVBNMQWERTYUASDGFHGDSFDSSDF").x, ImGui::GetFontSize() * 2.5);

	std::vector<std::string> lines;

	ImGuiContext& g = *GImGui;

	ImGui::GetStyle().WindowPadding = ImVec2(4, 10);
	ImGuiIO &io = ImGui::GetIO();

	MakeChildForList();

	std::string line = m_putf8Info; 

	while(line.find('\n') != -1)
	{
		unsigned short int poss = line.find('\n');
		if(poss <= 0) break;

		std::string buffer;
		buffer.assign(line, 0, poss);
		line.erase(0, poss + 1);

		if(buffer[0] != '\0')
			lines.push_back(buffer);

		if(poss <= 0) break;
	}

	if(line[0] != '\0')
		lines.push_back(line);

	ImGui::GetStyle().ButtonTextAlign = ImVec2(0.0f, 0.5f);
	ImGui::GetStyle().ItemSpacing = ImVec2(0.0f, 0.0f);

	unsigned int pSize = lines.size();

	for(int i = 1; i < pSize; i++)
	{	
		ImGuiStyle style;
		style.Colors[ImGuiCol_Button] = ImGui::GetStyle().Colors[ImGuiCol_Button];

		// first touch
		if(m_selectedItem == i) ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(RGBA_TO_FLOAT(155, 34, 34, 255)); 

		if(m_selectedItem == 0 && i == 1)
		{
			m_selectedItem = i;
			ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(RGBA_TO_FLOAT(155, 34, 34, 255)); 
		}

		ImVec2 curpos = ImGui::GetCursorPos();
		std::string szidname;

		for(int x = 0; x < i; x++)
			szidname.insert(szidname.begin(), ' ');

		ImVec2 butSize;

		butSize.x = vecButSize.x;
		
		if( ImGui::Button((char*)szidname.c_str(), ImVec2(butSize.x, ImGui::GetFontSize() * 2.45f)))
		{
			if(m_selectedItem == i)
			{
				ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(0.0f, 0.0f, 0.0f, 0.95f);

				if(pNetGame) 
				{
					Show(false);
					pNetGame->SendDialogResponse(m_wDialogID, 1, i - 1, (char*)lines[i - 1].c_str());
				}

				return;
			}

			strcpy(szDialogInputBuffer, lines[i].c_str());
			m_selectedItem = i;
		}

		ImGui::ItemSize( ImVec2(0, 15) );

		ImVec2 newxtcurpos = ImGui::GetCursorPos();
		ImGui::SetCursorPos(ImVec2(curpos.x+pGUI->GetFontSize(), curpos.y+pGUI->GetFontSize()/2+3.0));
		TextWithColors(lines[i].c_str());
		ImGui::SetCursorPos(newxtcurpos);
		
		ImGui::GetStyle().Colors[ImGuiCol_Button] = style.Colors[ImGuiCol_Button];
	}

	ImGui::GetStyle().ButtonTextAlign = ImVec2(0.5f,0.5f);
	// ImGui::GetStyle().FrameBorderSize = 0.5;
	ImGui::EndChild();
	ImGui::Dummy(ImVec2(1, pGUI->GetFontSize()));
	ImGui::GetStyle().WindowPadding = ImVec2(pGUI->GetFontSize(), pGUI->GetFontSize());
}

void CDialogWindow::Render()
{
	if(!m_bIsActive || !m_putf8Info) {
		return;
	}

	ImGuiIO &io = ImGui::GetIO();

	StyleColorsDialog(20.0f);

	ImVec2 buttsize;
	ImVec2 incurSize;
	buttsize.x = ImGui::CalcTextSize("QWERTYUIOPASQWERTYUIOPASQWERTY").x;
	incurSize.x = ImGui::CalcTextSize("QWERTYUIOPASDFGHJKLZXCVBN").x;

	ImVec2 inputSize;
	inputSize = ImVec2((ImGui::CalcTextSize("QWERTYUIOPAS").x) * 1.25f, pGUI->GetFontSize() * 2 + 3.0);
	int posInpX = (ImGui::GetWindowSize().x / 2) - (((inputSize.x*2) + pGUI->GetFontSize() + 0.25) / 2);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));

	if(ImGui::CalcTextSize(m_pszInfo).x > (io.DisplaySize.x/2)) 
		ImGui::Begin("dialog", nullptr, 
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_HorizontalScrollbar);
	else
		ImGui::Begin("dialog2", nullptr, 
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings);

	std::vector<std::string> lines;

	if(strlen(m_utf8Title) != 0)
		TextWithColors(m_utf8Title);
	
	if(m_byteDialogStyle != DIALOG_STYLE_LIST or m_byteDialogStyle != DIALOG_STYLE_TABLIST or m_byteDialogStyle != DIALOG_STYLE_TABLIST_HEADERS)
		ImGui::ItemSize( ImVec2(0, pGUI->GetFontSize()/2 + 2.5) );

	ImGui::GetStyle().ButtonTextAlign = ImVec2(0.5f, 0.5f);

	ImVec2 rsize = ImGui::GetWindowSize();

	float fx = io.DisplaySize.x * (1-0.55), fy = io.DisplaySize.y * (1-0.40);

	float inputX1;
		if(ImGui::CalcTextSize(m_pszInfo).x < 300) 
			inputX1 = 419.0f;
		else 
			inputX1 = ImGui::CalcTextSize(m_pszInfo).x * 1.4f + pGUI->GetFontSize() * 2;

	float curBorderSize = ImGui::GetStyle().FrameBorderSize;

	switch(m_byteDialogStyle)
	{
		case DIALOG_STYLE_MSGBOX:
		ImGui::ItemSize( ImVec2(0, 15) );
		TextWithColors(m_putf8Info);
		ImGui::Text("	");
		break;

		case DIALOG_STYLE_PASSWORD:
		case DIALOG_STYLE_INPUT:
		ImGui::ItemSize( ImVec2(0, 15) );
		TextWithColors(m_putf8Info);
		ImGui::ItemSize( ImVec2(0, 30) );

		ImGui::GetStyle().ButtonTextAlign = ImVec2(0.0f, 0.5f);

		ImGui::ItemSize( ImVec2(0, 35) );
		StyleColorsDialog(0.0f);
		if( ImGui::Button(utf8DialogInputBuffer, ImVec2(inputX1, ImGui::GetFontSize() * 2.35f)) )
		{
			if(pKeyBoard && !pKeyBoard->IsOpen()) {
				pKeyBoard->Open(&DialogWindowInputHandler);
			}
		}
		StyleColorsDialog(20.0f);
		// ImGui::GetStyle().FrameBorderSize = 0.0;

		ImGui::GetStyle().ButtonTextAlign = ImVec2(0.5f, 0.5f);
		
		ImGui::Text("	");
		break;

		case DIALOG_STYLE_TABLIST_HEADERS:
		StyleColorsDialog(0.0f);
		PutListHeadersTitle();
		ImGui::ItemSize( ImVec2(0, 5) );
		ImGui::GetStyle().FrameBorderSize = 0.0f;
		ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = ImVec4(RGBA_TO_FLOAT(155, 34, 34, 255));
		ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = ImVec4(RGBA_TO_FLOAT(155, 34, 34, 255));
		MakeListHeaders();
		ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = ImVec4(RGBA_TO_FLOAT(19, 19, 19, 0xFF));
		ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = ImVec4(RGBA_TO_FLOAT(60, 60, 60, 0xFF));
		ImGui::GetStyle().FrameBorderSize = curBorderSize;
		StyleColorsDialog(20.0f);
		ImGui::Text("	");
		break;

		case DIALOG_STYLE_TABLIST:
		case DIALOG_STYLE_LIST:
		StyleColorsDialog(0.0f);
		ImGui::GetStyle().FrameBorderSize = 0.0f;
		ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = ImVec4(RGBA_TO_FLOAT(155, 34, 34, 255));
		ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = ImVec4(RGBA_TO_FLOAT(155, 34, 34, 255));
		MakeList();
		ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered] = ImVec4(RGBA_TO_FLOAT(19, 19, 19, 0xFF));
		ImGui::GetStyle().Colors[ImGuiCol_ButtonActive] = ImVec4(RGBA_TO_FLOAT(60, 60, 60, 0xFF));
		ImGui::GetStyle().FrameBorderSize = curBorderSize;
		StyleColorsDialog(20.0f);
		ImGui::Text("	");
		break;
	}

	ImGui::Text("	");

	ImVec2 button1Size = ImVec2((ImGui::CalcTextSize("QWERTYUIOPAS").x) * 1.25f, pGUI->GetFontSize() * 2);
	ImVec2 button2Size = ImVec2((ImGui::CalcTextSize("QWERTYUIOPAS").x) * 1.25f, pGUI->GetFontSize() * 2);
	ImVec2 buttonSize = ImMax(button1Size, button2Size);

	int posx = ((ImGui::GetWindowSize().x / 2) - ((buttonSize.x + buttonSize.x + pGUI->GetFontSize() * 3) / 2) + 8.75);
	ImGui::GetStyle().ButtonTextAlign = ImVec2(0.5f, 0.5f);

	if(m_byteDialogStyle == DIALOG_STYLE_INPUT || m_byteDialogStyle == DIALOG_STYLE_PASSWORD)
	{
		ImGui::SetWindowSize(ImVec2(-1, -1));
	}
	else
	{
		if(ImGui::CalcTextSize(m_pszInfo).x < 478.0f && m_byteDialogStyle != DIALOG_STYLE_LIST && m_byteDialogStyle != DIALOG_STYLE_TABLIST && m_byteDialogStyle != DIALOG_STYLE_TABLIST_HEADERS)
			ImGui::SetWindowSize(ImVec2(500, -1));
		else
			ImGui::SetWindowSize(ImVec2(-1, -1));
	}

	ImVec2 size = ImGui::GetWindowSize();

	float savedCursor = ImGui::GetCursorPos().y;

	if(m_utf8Button1[0] != 0 && m_utf8Button2[0] != 0) 
	{
		float result = (ImGui::CalcTextSize("QWERTYUIOPAS").x + pGUI->GetFontSize()) + (ImGui::CalcTextSize("QWERTYUIOPAS").x + pGUI->GetFontSize());
		ImGui::SetCursorPosX((size.x - result - pGUI->GetFontSize()) / 2);
	}
	else
	{
		float result = (ImGui::CalcTextSize("QWERTYUIOPAS").x + pGUI->GetFontSize());
		ImGui::SetCursorPosX((size.x - result - pGUI->GetFontSize()) / 2);
	}

	ImGui::SetCursorPosY(ImGui::GetCursorPos().y - pGUI->ScaleY(25.0f));

	if(m_utf8Button1[0] != 0) 
	{
		ImVec2 butNewSize = ImVec2(ImGui::CalcTextSize("QWERTYUIOPAS").x + pGUI->GetFontSize(), pGUI->GetFontSize()*2.45f);

		if(ImGui::Button(m_utf8Button1, butNewSize))
		{
			if(pNetGame) 
			{
				if(m_byteDialogStyle == DIALOG_STYLE_TABLIST_HEADERS && m_selectedItem != 0) m_selectedItem -= 1;
				pNetGame->SendDialogResponse(m_wDialogID, 1, m_selectedItem, szDialogInputBuffer);
				Show(false);
			}
		}
	}

	ImGui::SameLine(0, pGUI->GetFontSize());

	if(m_utf8Button2[0] != 0) 
	{
		ImVec2 butNewSize = ImVec2(ImGui::CalcTextSize("QWERTYUIOPAS").x + pGUI->GetFontSize(), pGUI->GetFontSize()*2.45f);

		if(ImGui::Button(m_utf8Button2, butNewSize))
		{
			if(pNetGame) 
			{
				pNetGame->SendDialogResponse(m_wDialogID, 0, -1, szDialogInputBuffer);
				Show(false);
			}
		}
	}

	ImGui::SetCursorPosY(savedCursor);

	ImGui::Dummy(ImVec2(1, pGUI->GetFontSize()/1.15f));
		
	ImGui::SetWindowPos( ImVec2( ((io.DisplaySize.x - size.x)/2), ((io.DisplaySize.y - size.y)/2)) );

	ImGui::End();

	ImGui::PopStyleVar();
}
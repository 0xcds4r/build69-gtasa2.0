#include "main.h"
#include "gui/gui.h"
#include "chatwindow.h"
#include "keyboard.h"
#include "settings.h"
#include "game/game.h"
#include "net/netgame.h"
#include "game/game.h"
#include "dialog.h"

extern CGUI *pGUI;
extern CKeyBoard *pKeyBoard;
extern CSettings *pSettings;
extern CNetGame *pNetGame;
extern CGame *pGame;
extern CDialogWindow *pDialogWindow;

#define OPTIMAL_SLIDE_CHAT_TIME 			425
#define OPTIMAL_SLIDE_CHAT_MULT 			30

#define OPTIMAL_HIDE_CHAT_TIME 				7000
#define OPTIMAL_HIDE_CHAT_WAIT_TIME 		5000
#define OPTIMAL_HIDE_CHAT_MULT 				30
#define OPTIMAL_HIDE_CHAT_MULTPSR 			70

#define CHAT_SLIDE_MAX 						2.5f
#define CHAT_SLIDE_MIN 						0.5f

bool bToggleHud = false;
bool bNeedScroll = false;
bool bLineExec = false;
bool bNeedFastScroll = false;

int g_iCurrentSlideStep = 0;

uint32_t dwLastHideExecTick = GetTickCount();
uint32_t dwLastExecTick = GetTickCount();

void ChatWindowInputHandler(const char* str)
{
	if(!str || *str == '\0') {
		return;
	}

	if(!pNetGame) {
		return;
	}

	if(*str == '/') 
	{
		pNetGame->SendChatCommand(str);
	}
	else 
	{
		pNetGame->SendChatMessage(str);
	}

	return;
}

CChatWindow::CChatWindow()
{
	FLog("Initializing Chat Window..");

	m_fChatPosX = pGUI->ScaleX( 325.0f );
	m_fChatPosY = pGUI->ScaleY( 30.0f );
	m_fChatSizeX = pGUI->ScaleX( 1000.0f );
	m_fChatSizeY = pGUI->ScaleY( 220.0f );

	FLog("Chat pos: %f, %f, size: %f, %f", m_fChatPosX, m_fChatPosY, m_fChatSizeX, m_fChatSizeY);

	m_bChatHidden = false;
	m_iMaxMessages = 8;
	
	m_dwTextColor = 0xFFFFFFFF;
	m_dwInfoColor = 0x00C8C8FF;
	m_dwDebugColor = 0xA0C2DEFF;

	m_dwLastMessageUpdateTick = GetTickCount();
}

void CChatWindow::ProcessShowing()
{
	ImGui::GetStyle().Alpha = 1.0f;
	m_bChatHidden = false;
}

void CChatWindow::ProcessHidding()
{
	if(m_bChatHidden && GetTickCount() - m_dwLastMessageUpdateTick >= (OPTIMAL_HIDE_CHAT_TIME + OPTIMAL_HIDE_CHAT_MULTPSR)) 
	{
		if(m_bChatHidden && GetTickCount() - m_dwLastMessageUpdateTick >= (OPTIMAL_HIDE_CHAT_TIME + OPTIMAL_HIDE_CHAT_MULTPSR)+(OPTIMAL_HIDE_CHAT_MULT*1)) 
		{
			if(m_bChatHidden && GetTickCount() - m_dwLastMessageUpdateTick >= (OPTIMAL_HIDE_CHAT_WAIT_TIME + (OPTIMAL_HIDE_CHAT_TIME + OPTIMAL_HIDE_CHAT_MULTPSR))+(OPTIMAL_HIDE_CHAT_MULT*2)) 
			{
				if(m_bChatHidden && GetTickCount() - m_dwLastMessageUpdateTick >= (OPTIMAL_HIDE_CHAT_WAIT_TIME + (OPTIMAL_HIDE_CHAT_TIME + OPTIMAL_HIDE_CHAT_MULTPSR))+(OPTIMAL_HIDE_CHAT_MULT*3)) 
				{
					if(m_bChatHidden && GetTickCount() - m_dwLastMessageUpdateTick >= (OPTIMAL_HIDE_CHAT_WAIT_TIME + (OPTIMAL_HIDE_CHAT_TIME + OPTIMAL_HIDE_CHAT_MULTPSR))+(OPTIMAL_HIDE_CHAT_MULT*4)) 
					{
						if(m_bChatHidden && GetTickCount() - m_dwLastMessageUpdateTick >= (OPTIMAL_HIDE_CHAT_WAIT_TIME + (OPTIMAL_HIDE_CHAT_TIME + OPTIMAL_HIDE_CHAT_MULTPSR))+(OPTIMAL_HIDE_CHAT_MULT*5)) 
						{
							if(m_bChatHidden && GetTickCount() - m_dwLastMessageUpdateTick >= (OPTIMAL_HIDE_CHAT_WAIT_TIME + (OPTIMAL_HIDE_CHAT_TIME + OPTIMAL_HIDE_CHAT_MULTPSR))+(OPTIMAL_HIDE_CHAT_MULT*6)) 
							{
								if(m_bChatHidden && GetTickCount() - m_dwLastMessageUpdateTick >= (OPTIMAL_HIDE_CHAT_WAIT_TIME + (OPTIMAL_HIDE_CHAT_TIME + OPTIMAL_HIDE_CHAT_MULTPSR))+(OPTIMAL_HIDE_CHAT_MULT*7)) 
								{
									if(m_bChatHidden && GetTickCount() - m_dwLastMessageUpdateTick >= (OPTIMAL_HIDE_CHAT_WAIT_TIME + (OPTIMAL_HIDE_CHAT_TIME + OPTIMAL_HIDE_CHAT_MULTPSR))+(OPTIMAL_HIDE_CHAT_MULT*8)) 
									{
										ImGui::GetStyle().Alpha = 0.0f;
										return;
									}

									ImGui::GetStyle().Alpha = 0.1f;
									return;
								}

								ImGui::GetStyle().Alpha = 0.2f;
								return;
							}

							ImGui::GetStyle().Alpha = 0.3f;
							return;
						}

						ImGui::GetStyle().Alpha = 0.4f;
						return;
					}

					ImGui::GetStyle().Alpha = 0.5f;
					return;
				}

				ImGui::GetStyle().Alpha = 0.6f;
				return;
			}

			ImGui::GetStyle().Alpha = 0.75f;
			return;
		}

		ImGui::GetStyle().Alpha = 0.8f;
		return;
	}

	ImGui::GetStyle().Alpha = 0.9f;
	m_bChatHidden = true;
}

bool CChatWindow::Process()
{
	if(m_bChatHidden && GetTickCount() - m_dwLastMessageUpdateTick <= OPTIMAL_HIDE_CHAT_TIME) 
	{
		ProcessShowing();
	}
	else if(GetTickCount() - m_dwLastMessageUpdateTick > (OPTIMAL_HIDE_CHAT_TIME + OPTIMAL_HIDE_CHAT_MULT))
	{
		ProcessHidding();
	}
	
	return true;
}

void CChatWindow::ProcessScrolling()
{
	if(!bLineExec) 
	{
		g_iCurrentSlideStep = 0;
		dwLastExecTick = GetTickCount();
		bLineExec = true;
	}
	else
	{
		if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME && g_iCurrentSlideStep == 0)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); 
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+OPTIMAL_SLIDE_CHAT_MULT && g_iCurrentSlideStep == 1)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); // 1
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*2) && g_iCurrentSlideStep == 2)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); 
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*3) && g_iCurrentSlideStep == 3)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); // 2
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*4) && g_iCurrentSlideStep == 4)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); 
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*5) && g_iCurrentSlideStep == 5)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); // 3
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*6) && g_iCurrentSlideStep == 6)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); 
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*7) && g_iCurrentSlideStep == 7)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); //4
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*8) && g_iCurrentSlideStep == 8)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); 
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*9) && g_iCurrentSlideStep == 9)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); // 5
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*10) && g_iCurrentSlideStep == 10)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); // 1
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*11) && g_iCurrentSlideStep == 11)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); 
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*12) && g_iCurrentSlideStep == 12)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); // 2
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*13) && g_iCurrentSlideStep == 13)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); 
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*14) && g_iCurrentSlideStep == 14)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); // 3
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*15) && g_iCurrentSlideStep == 15)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); 
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*16) && g_iCurrentSlideStep == 16)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); //4
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*17) && g_iCurrentSlideStep == 17)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); 
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*18) && g_iCurrentSlideStep == 18)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); // 5
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*19) && g_iCurrentSlideStep == 19)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); // 1
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*20) && g_iCurrentSlideStep == 20)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); 
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*21) && g_iCurrentSlideStep == 21)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); // 2
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*22) && g_iCurrentSlideStep == 22)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); 
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*23) && g_iCurrentSlideStep == 23)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); // 3
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*24) && g_iCurrentSlideStep == 24)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); 
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*25) && g_iCurrentSlideStep == 25)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); //4
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*26) && g_iCurrentSlideStep == 26)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); 
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*27) && g_iCurrentSlideStep == 27)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); // 5
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*28) && g_iCurrentSlideStep == 28)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); // 1
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*29) && g_iCurrentSlideStep == 29)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); 
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*30) && g_iCurrentSlideStep == 30)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); // 2
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*31) && g_iCurrentSlideStep == 31)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); 
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*32) && g_iCurrentSlideStep == 32)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); // 3
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*33) && g_iCurrentSlideStep == 33)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); 
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*34) && g_iCurrentSlideStep == 34)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); //4
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*35) && g_iCurrentSlideStep == 35)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); 
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*36) && g_iCurrentSlideStep == 36)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); // 5
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*37) && g_iCurrentSlideStep == 37)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); // 1
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*38) && g_iCurrentSlideStep == 38)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); 
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*39) && g_iCurrentSlideStep == 39)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); // 2
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*40) && g_iCurrentSlideStep == 40)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); 
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*41) && g_iCurrentSlideStep == 41)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); // 3
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*42) && g_iCurrentSlideStep == 42)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); 
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*43) && g_iCurrentSlideStep == 43)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); //4
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*44) && g_iCurrentSlideStep == 44)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); 
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*45) && g_iCurrentSlideStep == 45)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); // 5
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*46) && g_iCurrentSlideStep == 46)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); // 1
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*47) && g_iCurrentSlideStep == 47)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); 
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*48) && g_iCurrentSlideStep == 48)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); // 2
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*49) && g_iCurrentSlideStep == 49)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); 
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*50) && g_iCurrentSlideStep == 50)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); // 3
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*51) && g_iCurrentSlideStep == 51)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); 
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*52) && g_iCurrentSlideStep == 52)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); //4
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*53) && g_iCurrentSlideStep == 53)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); 
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*54) && g_iCurrentSlideStep == 54)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN); // 5
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*55) && g_iCurrentSlideStep == 55)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MIN);
			g_iCurrentSlideStep++;
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*56) && g_iCurrentSlideStep == 56)
		{
			ImGui::SetScrollY(ImGui::GetScrollY() + CHAT_SLIDE_MAX);
		}
		else if(GetTickCount() - dwLastExecTick >= OPTIMAL_SLIDE_CHAT_TIME+(OPTIMAL_SLIDE_CHAT_MULT*57) && g_iCurrentSlideStep == 57)
		{
			ImGui::SetScrollHere();
			g_iCurrentSlideStep = 0;
			bLineExec = false;
			bNeedScroll = false;
		}
	}
}

bool CChatWindow::OnTouchEvent(int type, bool multi, int x, int y)
{
	static bool bWannaOpenChat = false;

	switch(type)
	{
		case TOUCH_PUSH:
			if (x >= m_fChatPosX && x <= m_fChatPosX + m_fChatSizeX &&
				y >= m_fChatPosY && y <= m_fChatPosY + m_fChatSizeY) 
			{
				bWannaOpenChat = true;
			}
		break;

		case TOUCH_POP:
			if(bWannaOpenChat &&
				x >= m_fChatPosX && x <= m_fChatPosX + m_fChatSizeX &&
				y >= m_fChatPosY && y <= m_fChatPosY + m_fChatSizeY)
			{	
				if(pKeyBoard)
				{
					if(!pKeyBoard->IsOpen()) 
					{
						if(pDialogWindow)
						{
							if(!pDialogWindow->IsShowing())
							{
								if(pNetGame)
								{
									pKeyBoard->Open(&ChatWindowInputHandler);
									m_dwLastMessageUpdateTick = GetTickCount();
								}
							}
						}
					}
				}
			}

			bWannaOpenChat = false;
		break;

		case TOUCH_MOVE:
		break;
	}

	return true;
}

float GTAFuncs_GetCameraRotation(float x, float y)
{
	ENTITY_TYPE* m_pEntity = (ENTITY_TYPE*)GamePool_FindPlayerPed();
	
	if(!m_pEntity) {
		return 0.0f;
	}

	MATRIX4X4 mat = *(MATRIX4X4*)((uintptr_t)m_pEntity + 20);
	
	// CGeneral::GetAngleBetweenPoints
	float fCamRot = reinterpret_cast<float(*)(float, float, float, float)>
	(g_GTASAAdr+0x3F4870+1)
	(mat.pos.X, mat.pos.Y, x, y);

	if(fCamRot > 360.0f) {
		fCamRot -= 360.0f;
	}

	if(fCamRot < 0.0f) {
		fCamRot += 360.0f;
	}

	return (float)fCamRot;
}

void CChatWindow::RenderWhenInput()
{
	if(pGame && pGame->GetCamera())
 	{
 		if(pDialogWindow && !pDialogWindow->IsShowing()) 
 		{
 			pGame->FindPlayerPed()->SetControllable(0);
 		}
 	}

	ImGuiIO &io = ImGui::GetIO();
	ImGui::GetStyle().WindowPadding = ImVec2(4, 12);
	ImGui::GetStyle().WindowRounding = 10.0f;

	ImGui::SetNextWindowBgAlpha(0.0f);
	ImGui::Begin("chat#idid232", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysVerticalScrollbar);
	
	ImGui::SetWindowSize(ImVec2(io.DisplaySize.x - pGUI->GetFontSize()*4.85f, m_fChatSizeY + pGUI->GetFontSize())); //  io.DisplaySize.y * (1-0.55)
	ImGui::SetWindowPos(ImVec2(pGUI->ScaleX(15.0f), m_fChatPosY));

	ImVec2 pos = ImVec2(m_fChatPosX, m_fChatPosY);

	for(auto entry : m_ChatWindowEntries2)
	{
		switch(entry.eType)
		{
			case CHAT_TYPE_CHAT:
				if(entry.szNick[0] != 0)
				{
					ImGui::TextColored(ImColor(entry.dwNickColor), "%s", entry.szNick);
					ImGui::SameLine();
				}

				ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(entry.dwTextColor));
				TextWithColors("%s", entry.utf8Message);
				ImGui::PopStyleColor();
			break;

			case CHAT_TYPE_INFO:
			case CHAT_TYPE_DEBUG:
				ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(entry.dwTextColor));
				TextWithColors("%s", entry.utf8Message);
				ImGui::PopStyleColor();
			break;
		}
	}

	if(!bToggleHud)
	{	
		if(pGame) 
		{
			ImGui::SetScrollHere();
			pGame->DisplayHUD(false);
		}
		bToggleHud = true;
	}
	else
	{
		if(bNeedScroll) 
		{
			ProcessScrolling();
		}
	}

	ImGui::End();
	ImGui::GetStyle().WindowPadding = ImVec2(13, 4);
	ImGui::GetStyle().WindowRounding = 0.0f;
}

void CChatWindow::RenderWhenFree()
{
	ImGuiIO &io = ImGui::GetIO();
	ImGui::GetStyle().WindowPadding = ImVec2(4, 12);
	ImGui::GetStyle().WindowRounding = 10.0f;

	ImGui::SetNextWindowBgAlpha(0.0f);
	ImGui::Begin("chat#ididDAWSD", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysVerticalScrollbar);
	
	ImGui::SetWindowSize(ImVec2(io.DisplaySize.x - pGUI->GetFontSize()*4.85f, m_fChatSizeY + pGUI->GetFontSize())); //  io.DisplaySize.y * (1-0.55)
	ImGui::SetWindowPos(ImVec2(345.0f, m_fChatPosY));

	ImVec2 pos = ImVec2(m_fChatPosX, m_fChatPosY);

	for(auto entry : m_ChatWindowEntries2)
	{
		switch(entry.eType)
		{
			case CHAT_TYPE_CHAT:
				if(entry.szNick[0] != 0)
				{
					ImGui::TextColored(ImColor(entry.dwNickColor), "%s", entry.szNick);
					ImGui::SameLine();
				}

				ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(entry.dwTextColor));
				TextWithColors("%s", entry.utf8Message);
				ImGui::PopStyleColor();
			break;

			case CHAT_TYPE_INFO:
			case CHAT_TYPE_DEBUG:
				ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(entry.dwTextColor));
				TextWithColors("%s", entry.utf8Message);
				ImGui::PopStyleColor();
			break;
		}
	}

	if(bNeedFastScroll) 
	{
		ImGui::SetScrollHere();
		bNeedFastScroll = false;
	}

	if(bToggleHud)
	{	
		if(pGame) 
		{
			ImGui::SetScrollHere();
			pGame->DisplayHUD(true);
		}
		bToggleHud = false;
	}
	else
	{
		if(bNeedScroll) 
		{
			ProcessScrolling();
		}
	}

	ImGui::End();
	ImGui::GetStyle().WindowPadding = ImVec2(13, 4);
	ImGui::GetStyle().WindowRounding = 0.0f;
}

void CChatWindow::Render()
{
	if(pKeyBoard && pDialogWindow)
	{
		if(pKeyBoard->IsOpen() && !pDialogWindow->IsShowing())
		{
			m_dwLastMessageUpdateTick = GetTickCount();

			RenderWhenInput();

			return;
		}

		if(!pDialogWindow->IsShowing()) 
		{
			Process();
			RenderWhenFree();
		}
		else
		{
			m_dwLastMessageUpdateTick = GetTickCount();
		}
	}
}

void CChatWindow::RenderText(const char* u8Str, float posX, float posY, uint32_t dwColor)
{
	const char* textStart = u8Str;
	const char* textCur = u8Str;
	const char* textEnd = u8Str + strlen(u8Str);

	ImVec2 posCur = ImVec2(posX, posY);
	ImColor colorCur = ImColor(dwColor);
	ImVec4 col;

	while(*textCur)
	{
		// {BBCCDD}
		// '{' и '}' соответствуют ASCII кодировке
		if(textCur[0] == '{' && ((&textCur[7] < textEnd) && textCur[7] == '}'))
		{
			// Выводим текст до фигурной скобки
			if(textCur != textStart)
			{
				// Выводим до текущего символа
				pGUI->RenderBGText(pGUI->GetFontSize(), posCur, colorCur, true, textStart, textCur);

				// Высчитываем новое смещение
				posCur.x += ImGui::CalcTextSize(textStart, textCur).x;
			}

			// Получаем цвет
			if(ProcessInlineHexColor(textCur+1, textCur+7, col))
				colorCur = col;

			// Двигаем смещение
			textCur += 7;
			textStart = textCur + 1;
		}

		textCur++;
	}

	if(textCur != textStart)
		pGUI->RenderBGText(pGUI->GetFontSize(), posCur, colorCur, true, textStart, textCur);

	return;
}

void CChatWindow::AddChatMessage(char* szNick, uint32_t dwNickColor, char* szMessage)
{
	FilterInvalidChars(szMessage);
	AddToChatWindowBuffer(CHAT_TYPE_CHAT, szMessage, szNick, m_dwTextColor, dwNickColor);
}

void CChatWindow::AddInfoMessage(char* szFormat, ...)
{
	char tmp_buf[512];
	memset(tmp_buf, 0, sizeof(tmp_buf));

	va_list args;
	va_start(args, szFormat);
	vsprintf(tmp_buf, szFormat, args);
	va_end(args);

	FilterInvalidChars(tmp_buf);
	AddToChatWindowBuffer(CHAT_TYPE_INFO, tmp_buf, nullptr, m_dwInfoColor, 0);
}

bool bTraugh = false;
void CChatWindow::AddDebugMessage(char *szFormat, ...)
{
	if(!bTraugh) 
	{
		for(int i = 0; i<=m_iMaxMessages; i++) 
		{
			char tmp_buf[512];
			memset(tmp_buf, 0, sizeof(tmp_buf));

			va_list args;
			va_start(args, " ");
			vsprintf(tmp_buf, " ", args);
			va_end(args);
		
			FilterInvalidChars(tmp_buf);
			AddToChatWindowBuffer(CHAT_TYPE_DEBUG, tmp_buf, nullptr, m_dwDebugColor, 0);	
		}

		bTraugh = true;
		bNeedFastScroll = true;
	}

	char tmp_buf[512];
	memset(tmp_buf, 0, sizeof(tmp_buf));

	va_list args;
	va_start(args, szFormat);
	vsprintf(tmp_buf, szFormat, args);
	va_end(args);

	FilterInvalidChars(tmp_buf);
	AddToChatWindowBuffer(CHAT_TYPE_DEBUG, tmp_buf, nullptr, m_dwDebugColor, 0);	
}

void CChatWindow::AddClientMessage(uint32_t dwColor, char* szStr)
{
	FilterInvalidChars(szStr);
	AddToChatWindowBuffer(CHAT_TYPE_INFO, szStr, nullptr, dwColor, 0);
}

void CChatWindow::PushBack(CHAT_WINDOW_ENTRY &entry)
{
	if(m_ChatWindowEntries.size() >= m_iMaxMessages)
		m_ChatWindowEntries.pop_front();

	m_ChatWindowEntries.push_back(entry);

	if(m_ChatWindowEntries2.size() >= 0xFFFFFFFF)
		m_ChatWindowEntries2.pop_front();

	m_ChatWindowEntries2.push_back(entry);

	return;
}

void CChatWindow::AddToChatWindowBuffer(eChatMessageType type, char* szString, char* szNick, 
	uint32_t dwTextColor, uint32_t dwNickColor)
{
	int iBestLineLength = 0;
	CHAT_WINDOW_ENTRY entry;
	entry.eType = type;
	entry.dwNickColor = __builtin_bswap32(dwNickColor | 0x000000FF);
	entry.dwTextColor = __builtin_bswap32(dwTextColor | 0x000000FF);

	if(szNick)
	{
		strcpy(entry.szNick, szNick);
		strcat(entry.szNick, ":");
	}
	else 
		entry.szNick[0] = '\0';

	if(type == CHAT_TYPE_CHAT && strlen(szString) > MAX_LINE_LENGTH)
	{
		iBestLineLength = MAX_LINE_LENGTH;

		while(szString[iBestLineLength] != ' ' && iBestLineLength) {
			iBestLineLength--;
		}

		if((MAX_LINE_LENGTH - iBestLineLength) > 12)
		{
			cp1251_to_utf8(entry.utf8Message, szString, MAX_LINE_LENGTH);
			PushBack(entry);

			entry.szNick[0] = '\0';
			cp1251_to_utf8(entry.utf8Message, szString+MAX_LINE_LENGTH);
			PushBack(entry);
		}
		else
		{
			cp1251_to_utf8(entry.utf8Message, szString, iBestLineLength);
			PushBack(entry);

			entry.szNick[0] = '\0';
			cp1251_to_utf8(entry.utf8Message, szString+(iBestLineLength+1));
			PushBack(entry);
		}
	}
	else
	{
		cp1251_to_utf8(entry.utf8Message, szString, MAX_MESSAGE_LENGTH);
		PushBack(entry);
	}

	m_dwLastMessageUpdateTick = GetTickCount();
	bNeedScroll = true;

	return;
}

void CChatWindow::FilterInvalidChars(char *szString)
{
	while(*szString)
	{
		if(*szString > 0 && *szString < ' ')
			*szString = ' ';

		szString++;
	}
}
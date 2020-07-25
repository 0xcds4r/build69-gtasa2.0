#pragma once

#include "game/textdraw.h"

#define MAX_TEXT_DRAWS  2304

class CTextDrawPool
{
private:
    CTextDraw           *m_pTextDraw[MAX_TEXT_DRAWS];
    bool                 m_bSlotState[MAX_TEXT_DRAWS];
    bool                 m_bSelectState;
	uint32_t             m_dwHoverColor;
    uint16_t             m_wClickedTextDrawID;

public:
    CTextDrawPool();
    ~CTextDrawPool();

    CTextDraw *New(unsigned short wText, TEXT_DRAW_TRANSMIT *TextDrawTransmit, char *szText);
    void Delete(unsigned short wText);
    void Draw();

    bool OnTouchEvent(int type, bool multi, int x, int y);
    void SendClick();
    void SetSelectState(bool bState, uint32_t dwColor);

    void SnapshotProcess();

    bool GetState() { return m_bSelectState; };

    CTextDraw *GetAt(unsigned short wText) {
        if(wText >= MAX_TEXT_DRAWS) return 0;
        if(!m_bSlotState[wText]) return 0;
        return m_pTextDraw[wText];
    }
};
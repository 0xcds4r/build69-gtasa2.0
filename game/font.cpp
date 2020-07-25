#include "main.h"
#include "font.h"

void CFont::AsciiToGxtChar(const char* ascii, uint16_t* gxt)
{
	return ((void(*)(const char*, uint16_t*))(g_GTASAAdr + 0x5A8351))(ascii, gxt);
}

void CFont::SetScale(float x, float y)
{
	*(float*)(g_GTASAAdr + 0xA297BC) = y;
	*(float*)(g_GTASAAdr + 0xA297B8) = x;
}

void CFont::SetColor(uint32_t *dwColor)
{
	return ((void(*)(uint32_t*))(g_GTASAAdr + 0x5AAFC9))(dwColor);
}

void CFont::SetJustify(uint8_t justify)
{
	return ((void(*)(uint8_t))(g_GTASAAdr + 0x5AB2F5))(justify);
}

void CFont::SetOrientation(uint8_t orientation)
{
	return ((void(*)(uint8_t))(g_GTASAAdr + 0x5AB305))(orientation);
}

void CFont::SetWrapX(float wrapX)
{
	return ((void(*)(float))(g_GTASAAdr + 0x5AB1D9))(wrapX);
}

void CFont::SetCentreSize(float size)
{
	return ((void(*)(float))(g_GTASAAdr + 0x5AB1E9))(size);
}

void CFont::SetBackground(uint8_t bBackground, uint8_t bOnlyText)
{
	return ((void(*)(uint8_t, uint8_t))(g_GTASAAdr + 0x5AB2C1))(bBackground, bOnlyText);
}

void CFont::SetBackgroundColor(uint32_t *dwColor)
{
	return ((void(*)(uint32_t*))(g_GTASAAdr + 0x5AB2D1))(dwColor);
}

void CFont::SetProportional(uint8_t prop)
{
	return ((void(*)(uint8_t))(g_GTASAAdr + 0x5AB2B1))(prop);
}

void CFont::SetDropColor(uint32_t* dwColor)
{
	return ((void(*)(uint32_t*))(g_GTASAAdr + 0x5AB219))(dwColor);
}

void CFont::SetDropShadowPosition(uint8_t pos)
{
	return ((void(*)(uint8_t))(g_GTASAAdr + 0x5A8A6D))(pos);
}

void CFont::PrintString(float posX, float posY, const char *string )
{
	uint16_t *gxt_string = new uint16_t[0xFF];

	CFont::AsciiToGxtChar(string, gxt_string);

	// CFont::PrintString
	(( void (*)(float, float, uint16_t*))(g_GTASAAdr+0x5AA191))(posX, posY, gxt_string);
	
	delete gxt_string;

	// CFont::RenderFontBuffer
	(( void (*)())(g_GTASAAdr+0x5A90B0+1))();

	CFont::SetEdge(0);
}

void CFont::SetFontStyle(uint8_t style)
{
	return ((void(*)(uint8_t))(g_GTASAAdr + 0x5AB14D))(style);
}

void CFont::SetEdge(uint8_t edge)
{
	return ((void(*)(uint8_t))(g_GTASAAdr + 0x5AB27D))(edge);
}

float CFont::GetStringWidth(uint16_t *string, bool unk1, bool unk2)
{
	float (*_GetStringWidth)(uint16_t *string, bool unk1, bool unk2);
    *(void **) (&_GetStringWidth) = (void*)(g_GTASAAdr+0x5AAB7C+1);
    return (*_GetStringWidth)(string, unk1, unk2);
}
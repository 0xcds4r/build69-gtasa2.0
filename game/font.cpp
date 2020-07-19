#include "../main.h"
#include "font.h"

void CFont::AsciiToGxtChar(const char* ascii, uint16_t* gxt)
{
	return (( void (*)(const char*, uint16_t*))(g_GTASAAdr+0x5A8350+1))(ascii, gxt);
}
#pragma once

#include "game/RW/RenderWare.h"

#define COLOR_WHITE		0xFFFFFFFF
#define COLOR_BLACK 	0xFF000000
#define COLOR_ORANGE 	0xFF00A5FF
#define COLOR_ROSE		0xFFFF99FF
#define COLOR_BRED		0xFF9933FF
#define COLOR_BLUE		0xFF6C2713
#define COLOR_CYAN		0xFFCE6816
#define COLOR_1			0xFFB58891
#define COLOR_2			0xFF673F40

struct stRect
{
	int x1;	// left
	int y1;	// top
	int x2;	// right
	int y2;	// bottom
};

struct stfRect
{
	float x1;
	float y1;
	float x2;
	float y2;
};

bool ProcessInlineHexColor(const char* start, const char* end, ImVec4& color);
std::string removeColorTags(std::string line);
ImVec2 CalcTextSizeWithoutTags(char* szStr);
void TextWithColors(const char* fmt, ...);
void TextWithColors(ImVec2 pos, ImColor col, const char* szStr);
void DrawRaster(stRect *rect, uint32_t color, RwRaster *raster = nullptr, stfRect *uv = nullptr);
void CreateBox(ImVec2 vecPos, ImVec2 vecSize, ImColor col);
void CreateBoxRect(ImVec2 vecPos, ImVec2 vecSize, ImColor col);
void CreateLine(ImVec2 vecPos, ImVec2 vecSize, ImColor col);
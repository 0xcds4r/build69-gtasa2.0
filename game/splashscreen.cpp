#include "../main.h"
#include "RW/RenderWare.h"
#include "game.h"
#include "gui/gui.h"

extern CGUI *pGUI;

RwTexture *splashTexture = nullptr;

#define MAX_SCHEMAS 4
uint32_t colors[MAX_SCHEMAS][2] = {
	{ COLOR_ROSE,	COLOR_BRED },
	{ COLOR_BLACK, 	COLOR_ORANGE },
	{ COLOR_CYAN,	COLOR_BLUE },
	{ COLOR_1,		COLOR_2 }
};
unsigned int color_scheme = 0;

void LoadSplashTexture()
{
	FLog("Loading splash texture..");
	splashTexture = (RwTexture*)LoadTextureFromDB("samp", "mylogo");

	color_scheme = 1;//rand() % MAX_SCHEMAS;
}

void RenderSplash()
{
	stRect rect;
	stfRect uv;
	stfRect sRect;

	// background
	rect.x1 = 0;
	rect.y1 = 0;
	rect.x2 = RsGlobal->maximumWidth;
	rect.y2 = RsGlobal->maximumHeight;
	DrawRaster(&rect, colors[color_scheme][0]);

	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)1);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);

	// texture
	rect.x1 = RsGlobal->maximumWidth * 0.05;
	rect.y1 = RsGlobal->maximumHeight * 0.25;
	rect.x2 = RsGlobal->maximumWidth - rect.x1;
	rect.y2 = RsGlobal->maximumHeight - rect.y1;
	uv.x1 = 0.0f;
	uv.y1 = 0.0f;
	uv.x2 = 1.0;
	uv.y2 = 1.0;
	DrawRaster(&rect, COLOR_WHITE, splashTexture->raster, &uv);

	const float percent = *(float*)(g_GTASAAdr+0x9920C0);
	if(percent <= 0.0f) return;
	float mult = percent/100.0f;
	// offset
	float newX = rect.x1 + (mult * 0.9 * RsGlobal->maximumWidth);

	sRect.x1 = (float)rect.x1; 	// x1
	sRect.y2 = (float)rect.y1; 	// y1
	sRect.x2 = (float)newX;		// x2
	sRect.y1 = (float)rect.y2;	// y2
	SetScissorRect((void*)&sRect);
	DrawRaster(&rect, colors[color_scheme][1], splashTexture->raster, &uv);

	sRect.x1 = 0.0f;
	sRect.y1 = 0.0f;
	sRect.x2 = 0.0f;
	sRect.y2 = 0.0f;
	SetScissorRect((void*)&sRect);
}

void ImGui_ImplRenderWare_RenderDrawData(ImDrawData* draw_data);
void ImGui_ImplRenderWare_NewFrame();

void RenderSplashScreen()
{
	RenderSplash();

	if(!pGUI) return;

	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplRenderWare_NewFrame();
	ImGui::NewFrame();

	ImGui::GetOverlayDrawList()->AddText(ImVec2(0, 0), colors[color_scheme][1], 
		"\n\t"
		u8"cds4r@smmandroid.ru\n\t"
		u8"vk.com/mobile.samp\n\t"
		u8"donationalerts.ru/r/cds4r");

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplRenderWare_RenderDrawData(ImGui::GetDrawData());
}
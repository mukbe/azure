#include "stdafx.h"
#include "Fog.h"



Fog::Fog()
{
	fogBuffer = new FogBuffer;
}


Fog::~Fog()
{
	SafeDelete(fogBuffer);
}

void Fog::Render()
{
	fogBuffer->SetPSBuffer(5);
}

void Fog::UIUpdate()
{
}

void Fog::UIRender()
{
	static bool isFogOn = true;
	if(ImGui::Checkbox("FogOn", &isFogOn))
	{
		if (isFogOn)
			fogBuffer->data.fogOn = 2.0f;
		else fogBuffer->data.fogOn = 0.0f;
	}
	ImGui::SliderFloat("FogStart", &fogBuffer->data.fogStart, 0.0f, 1000.0f);
	ImGui::SliderFloat("FogEnd", &fogBuffer->data.fogEnd, 0.0f, 1000.0f);
	ImGui::ColorEdit4("FogColor", (float*)&fogBuffer->data.fogColor.r,
		ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaPreviewHalf);
}

#include "stdafx.h"
#include "Inspector.h"

#include "ToolScene.h"
#include "Hierarchy.h"

#include "../TerrainTool/Terrain.h"
#include "../TerrainTool/Scattering.h"

#include "./View/FreeCamera.h"

Inspector::Inspector(ToolScene * toolScene)
	:ToolBase(toolScene)
{
	name = "Inspector";
}

Inspector::~Inspector()
{
}

void Inspector::Init()
{
	hierarchy = toolScene->GetTool<Hierarchy>("Hierarchy");
}

void Inspector::Release()
{
}

void Inspector::PreUpdate()
{
}

void Inspector::Update()
{
}

void Inspector::PostUpdate()
{
}

void Inspector::PreRender()
{
}

void Inspector::Render()
{
}

void Inspector::UIRender()
{
}

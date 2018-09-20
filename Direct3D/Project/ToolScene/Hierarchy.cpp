#include "stdafx.h"
#include "Hierarchy.h"

#include "ToolScene.h"

#include "../TerrainTool/Terrain.h"
#include "../TerrainTool/Scattering.h"

#include "./View/FreeCamera.h"

#include "./Environment/Ocean.h"

Hierarchy::Hierarchy(ToolScene * toolScene)
	:ToolBase(toolScene)
{
	name = "Hierarchy";

	freeCamera = new FreeCamera;
	Objects->SetMainCamera(freeCamera);
	scattering = new Scattering(freeCamera, "level");
	ocean = new Ocean();
}

Hierarchy::~Hierarchy()
{
}

void Hierarchy::Init()
{
	inspector = toolScene->GetTool<Inspector>("Inspector");
}

void Hierarchy::Release()
{
}

void Hierarchy::PreUpdate()
{
	
}

void Hierarchy::Update()
{
	freeCamera->Update();

	scattering->Updata();
	ocean->Update();
}

void Hierarchy::PostUpdate()
{
}

void Hierarchy::PreRender()
{
	
}

void Hierarchy::Render()
{
	freeCamera->Render();

	scattering->Render();
	ocean->Render();
}

void Hierarchy::UIRender()
{
	scattering->UIRender();
	ocean->UIRender();
}

#include "stdafx.h"
#include "TerrainTool.h"
#include "Terrain.h"

#include "./View/FreeCamera.h"

#include "Scattering.h"

TerrainTool::TerrainTool()
{
	RenderRequest->AddRender("TerrainToolUIRender", bind(&TerrainTool::UIRender, this), RenderType::UIRender);
	RenderRequest->AddRender("TerrainToolRender", bind(&TerrainTool::Render, this), RenderType::Render);

	terrain = new Terrain;
	freeCamera = new FreeCamera;
	sun = new Environment::Sun;
	test = new Scattering(freeCamera);
}


TerrainTool::~TerrainTool()
{
	Release();
}

void TerrainTool::Init()
{
}

void TerrainTool::Release()
{
}

void TerrainTool::PreUpdate()
{
	freeCamera->Render();
	terrain->PreUpdate();
}

void TerrainTool::Update()
{
	terrain->Update();
	test->Updata();
}

void TerrainTool::PostUpdate()
{
	terrain->PostUpdate();
	freeCamera->Update();
	//sun->UpdateView();
}

void TerrainTool::PreRender()
{
	terrain->PreRender();
	//camera정보를 deferred에게 언팩킹시에 필요한 정보를 보낸다
	RenderRequest->SetUnPackGBufferProp(freeCamera->GetViewMatrix(), freeCamera->GetProjection());

}

void TerrainTool::Render()
{
	//Bind Prop
	freeCamera->Render();
	//sun->Render();

	//Render
	test->Render();

	terrain->Render();
}

void TerrainTool::UIRender()
{
	terrain->UIRender();
	test->UIRender();
	AssetManager->UIRender();
}


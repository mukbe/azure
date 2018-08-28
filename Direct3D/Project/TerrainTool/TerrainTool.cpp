#include "stdafx.h"
#include "TerrainTool.h"
#include "Terrain.h"

#include "./View/FreeCamera.h"
#include "./Renders/DeferredRenderer.h"

TerrainTool::TerrainTool()
{
	RenderRequest->AddRender("TerrainToolUIRender", bind(&TerrainTool::UIRender, this), RenderType::UIRender);
	RenderRequest->AddRender("TerrainToolRender", bind(&TerrainTool::Render, this), RenderType::Render);

	terrain = new Terrain;
	freeCamera = new FreeCamera;
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
	terrain->PreUpdate();
}

void TerrainTool::Update()
{
	terrain->Update();
}

void TerrainTool::PostUpdate()
{
	terrain->PostUpdate();
	freeCamera->Update();
}

void TerrainTool::PreRender()
{
	terrain->PreRender();
}

void TerrainTool::Render()
{
	freeCamera->Render();

	terrain->Render();

	//camera정보를 deferred에게 언팩킹시에 필요한 정보를 보낸다
	DeferredRenderer*deferred = (DeferredRenderer*)RenderRequest->GetDeferred();
	deferred->SetUnPackInfo(freeCamera->GetViewMatrix(), freeCamera->GetProjection());

	States::SetSampler(1, States::LINEAR);


}

void TerrainTool::UIRender()
{
	terrain->UIRender();
}


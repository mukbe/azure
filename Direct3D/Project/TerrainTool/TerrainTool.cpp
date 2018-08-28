#include "stdafx.h"
#include "TerrainTool.h"
#include "Terrain.h"

#include "./View/FreeCamera.h"
#include "./Environment/Sun.h"

TerrainTool::TerrainTool()
{
	RenderRequest->AddRender("TerrainToolUIRender", bind(&TerrainTool::UIRender, this), RenderType::UIRender);
	RenderRequest->AddRender("TerrainToolRender", bind(&TerrainTool::Render, this), RenderType::Render);

	terrain = new Terrain;
	freeCamera = new FreeCamera;
	sun = new Environment::Sun;
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
	sun->UpdateView();
}

void TerrainTool::PreRender()
{
	terrain->PreRender();
	//camera������ deferred���� ����ŷ�ÿ� �ʿ��� ������ ������
	RenderRequest->SetUnPackGBufferProp(freeCamera->GetViewMatrix(), freeCamera->GetProjection());

}

void TerrainTool::Render()
{
	//Bind Prop
	freeCamera->Render();
	sun->Render();

	//Render
	terrain->Render();
}

void TerrainTool::UIRender()
{
	terrain->UIRender();
}


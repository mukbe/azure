#include "stdafx.h"
#include "TerrainTool.h"
#include "Terrain.h"

#include "./View/FreeCamera.h"

#include "Scattering.h"
#include "./Particle/ParticleSample.h"

TerrainTool::TerrainTool()
{
	RenderRequest->AddRender("TerrainToolUIRender", bind(&TerrainTool::UIRender, this), RenderType::UIRender);
	RenderRequest->AddRender("TerrainToolRender", bind(&TerrainTool::Render, this), RenderType::Render);

	terrain = new Terrain;
	freeCamera = new FreeCamera;
	sun = new Environment::Sun;
	test = new Scattering(freeCamera, "level");
	Particletest = new ParticleSample;

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
	test->Update();
	Particletest->Update();
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
	//camera������ deferred���� ����ŷ�ÿ� �ʿ��� ������ ������
	RenderRequest->SetUnPackGBufferProp(freeCamera->GetViewMatrix(), freeCamera->GetProjection());

}

void TerrainTool::Render()
{
	//Bind Prop
	freeCamera->Render();
	//sun->Render();

	//Render
	test->Render();
	Particletest->Render();
	terrain->Render();
}

void TerrainTool::UIRender()
{
	terrain->UIRender();
	test->UIRender();
	AssetManager->UIRender();
}


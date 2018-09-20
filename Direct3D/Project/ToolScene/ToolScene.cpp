#include "stdafx.h"
#include "ToolScene.h"

#include "ToolBase.h"
#include "Hierarchy.h"
#include "Inspector.h"

#include "../TerrainTool/Terrain.h"
#include "../TerrainTool/Scattering.h"

#include "./View/FreeCamera.h"

#include "./Environment/Ocean.h"


ToolScene::ToolScene()
{
	RenderRequest->AddRender("ToolUIRender", bind(&ToolScene::UIRender, this), RenderType::UIRender);
	RenderRequest->AddRender("ToolRender", bind(&ToolScene::Render, this), RenderType::Render);
	RenderRequest->AddRender("ToolPreRender", bind(&ToolScene::PreRender, this), RenderType::PreRender);

	toolList.insert(make_pair(ToolType::Hierarchy, new Hierarchy(this)));
	toolList.insert(make_pair(ToolType::Inspector, new Inspector(this)));

	ToolIter iter = toolList.begin();
	for (; iter != toolList.end(); ++iter)
		iter->second->Init();

}


ToolScene::~ToolScene()
{
	this->Release();
}

void ToolScene::Init()
{
}

void ToolScene::Release()
{
	ToolIter iter = toolList.begin();
	for (; iter != toolList.end(); ++iter)
	{
		iter->second->Release();
		SafeDelete(iter->second);
	}

	toolList.clear();
}

void ToolScene::PreUpdate()
{
	
}

void ToolScene::Update()
{
	ToolIter iter = toolList.begin();
	for (; iter != toolList.end(); ++iter)
	{
		iter->second->Update();
	}

}

void ToolScene::PostUpdate()
{
}

void ToolScene::PreRender()
{

}

void ToolScene::Render()
{
	ToolIter iter = toolList.begin();
	for (; iter != toolList.end(); ++iter)
	{
		iter->second->Render();
	}
}

void ToolScene::UIRender()
{
	ToolIter iter = toolList.begin();
	for (; iter != toolList.end(); ++iter)
	{
		iter->second->UIRender();
	}
}

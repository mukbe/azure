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
	Objects->PreUpdate();
}

void ToolScene::Update()
{
	ToolIter iter = toolList.begin();
	for (; iter != toolList.end(); ++iter)
	{
		iter->second->Update();
	}
	Objects->Update();
}

void ToolScene::PostUpdate()
{
	Objects->PostUpdate();
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

	Objects->Render();
}

void ToolScene::UIRender()
{
	static bool isHierarchyOn = true;
	static bool isInspectorOn = true;

	ImGui::BeginMainMenuBar();
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tool"))
		{
			if (ImGui::MenuItem("Hierarchy"))
				isHierarchyOn = !isHierarchyOn;
			if (ImGui::MenuItem("Inspector"))
				isInspectorOn = !isInspectorOn;
			
			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();

	if (isHierarchyOn)
		toolList[ToolType::Hierarchy]->UIRender();
	if (isInspectorOn)
		toolList[ToolType::Inspector]->UIRender();
}

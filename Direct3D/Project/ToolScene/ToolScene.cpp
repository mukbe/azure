#include "stdafx.h"
#include "ToolScene.h"

#include "ToolBase.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "FactoryTool.h"
#include "Picking.h"

#include "../TerrainTool/Terrain.h"
#include "../TerrainTool/Scattering.h"

#include "./View/FreeCamera.h"

#include "./Environment/Ocean.h"

#include "./Utilities/Transform.h"
#include "./Utilities/ImGuiHelper.h"

#include "./Figure/Figure.h"

#include "./Model/ComputeAnimation/ComputeAnimation.h"
#include "./Renders/GrassRenderer/GrassRenderer.h"
#include "./Environment/Fog.h"

ToolScene::ToolScene()
{
	Scenes->SetIsTool(true);
	DataBase::Get()->Load();

	RenderRequest->AddRender("ToolUIRender", bind(&ToolScene::UIRender, this), RenderType::UIRender);
	RenderRequest->AddRender("ToolRender", bind(&ToolScene::Render, this), RenderType::Render);
	RenderRequest->AddRender("ToolPreRender", bind(&ToolScene::PreRender, this), RenderType::PreRender);
	RenderRequest->AddRender("Particle", std::bind(&ToolScene::AlphaRender, this), RenderType::AlphaRender);

	toolList.insert(make_pair(ToolType::Hierarchy, new Hierarchy(this)));
	toolList.insert(make_pair(ToolType::Inspector, new Inspector(this)));
	toolList.insert(make_pair(ToolType::Factory, new FactoryTool(this)));
	toolList.insert(make_pair(ToolType::Unknown, new Picking(this)));

	ToolIter iter = toolList.begin();
	for (; iter != toolList.end(); ++iter)
		iter->second->Init();

	FreeCamera* camera = new FreeCamera;
	ObjectManager::Get()->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::System, camera);
	ObjectManager::Get()->ObjectSortingFront(ObjectType::Type::Dynamic, ObjectType::Tag::System, "Camera");
	ObjectManager::Get()->SetMainCamera(camera);

	ObjectManager::Get()->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::Instancing, new GrassRenderer);

	ObjectManager::Get()->LoadData(DataBase::Get()->GetValue());

	ObjectManager::Get()->Init();

	ObjectManager::Get()->AddObject(ObjectType::Type::Dynamic, ObjectType::Tag::Enviroment, new Fog);
}


ToolScene::~ToolScene()
{
	this->Release();
	DebugHelper::Delete();
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
	Objects->Update();

	ToolIter iter = toolList.begin();
	for (; iter != toolList.end(); ++iter)
	{
		iter->second->Update();
	}

	if (KeyCode->Down(VK_SPACE))
		DebugHelper::Log("Test Log");

}

void ToolScene::PostUpdate()
{
	Objects->PostUpdate();
}

void ToolScene::AlphaRender()
{
	//Objects->AlphaRender();
}

void ToolScene::PreRender()
{
	RenderRequest->SetUnPackGBufferProp(MainCamera->GetViewMatrix(), MainCamera->GetProjection());
}

void ToolScene::Render()
{
	Objects->Render();

	ToolIter iter = toolList.begin();
	for (; iter != toolList.end(); ++iter)
	{
		iter->second->Render();
	}

}

void ToolScene::UIRender()
{
	static bool isDemoOn = false;
	static bool isHierarchyOn = true;
	static bool isInspectorOn = true;
	static bool isFactory = false;
	static bool isAssetOn = false;
	static bool isDebugOn = false;

	ImGui::BeginMainMenuBar();
	{
		if (ImGui::BeginMenu("File"))
		{
			if(ImGui::MenuItem("Save"))
				DataBase::Get()->Save();
			if (ImGui::MenuItem("DebugLog"))
				isDebugOn = !isDebugOn;
			if (ImGui::MenuItem("Demo"))
				isDemoOn = !isDemoOn;
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tool"))
		{
			if (ImGui::MenuItem("Hierarchy"))
				isHierarchyOn = !isHierarchyOn;
			if (ImGui::MenuItem("Inspector"))
				isInspectorOn = !isInspectorOn;
			if (ImGui::MenuItem("Asset"))
				isAssetOn = !isAssetOn;
			if (ImGui::MenuItem("Factory"))
				isFactory = !isFactory;

			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();


	if (isDemoOn)
		ImGui::ShowDemoWindow();
	if (isDebugOn)
		DebugHelper::UIRender();
	if (isHierarchyOn)
		toolList[ToolType::Hierarchy]->UIRender();
	if (isInspectorOn)
		toolList[ToolType::Inspector]->UIRender();
	if (isFactory)
		toolList[ToolType::Factory]->UIRender();
	if (isAssetOn)
		AssetManager->UIRender();


}

void ToolScene::SaveData()
{
	ToolIter iter = toolList.begin();
	for (; iter != toolList.end(); ++iter)
		iter->second->SaveData();
}

void ToolScene::LoadData()
{
	ToolIter iter = toolList.begin();
	for (; iter != toolList.end(); ++iter)
		iter->second->LoadData();
}

#include "stdafx.h"
#include "PlayScene.h"

#include "../TerrainTool/Terrain.h"
#include "../TerrainTool/Scattering.h"

#include "./View/FreeCamera.h"

#include "./Environment/Ocean.h"

#include "./Renders/Instancing/InstanceRenderer.h"

#include "./Utilities/Transform.h"
#include "./Utilities/ImGuiHelper.h"

#include "./Figure/Figure.h"


PlayScene::PlayScene()
{
	RenderRequest->AddRender("ToolUIRender", bind(&PlayScene::UIRender, this), RenderType::UIRender);
	RenderRequest->AddRender("ToolRender", bind(&PlayScene::Render, this), RenderType::Render);
	RenderRequest->AddRender("ToolPreRender", bind(&PlayScene::PreRender, this), RenderType::PreRender);

	this->InitSoonwoo();
	this->InitHuynjin();

	Objects->Init();
}


PlayScene::~PlayScene()
{
}

void PlayScene::Init()
{
	
}

void PlayScene::Release()
{
}

void PlayScene::PreUpdate()
{
	Objects->PreUpdate();
}

void PlayScene::Update()
{
	Objects->Update();
}

void PlayScene::PostUpdate()
{
	Objects->PostUpdate();
}

void PlayScene::ShadowRender()
{
}

void PlayScene::PreRender()
{
	RenderRequest->SetUnPackGBufferProp(MainCamera->GetViewMatrix(), MainCamera->GetProjection());
}

void PlayScene::Render()
{
	Objects->Render();

	static bool isDebug = false;
	if (KeyCode->Down(VK_F1))
		ObjectManager::Get()->SetIsDebug(!Objects->GetIsDebug());

	if (Objects->GetIsDebug())
		Objects->DebugRender();


}

void PlayScene::UIRender()
{
	ImGui::Begin("Debug");

	ImGui::Text("FPS : %f", Time::Get()->FPS());

	Objects->FindObject(ObjectType::Type::Dynamic, ObjectType::Tag::System, "QuadTreeSystem")->UIRender();
	int objectCount = 0;
	vector<GameObject*>* pList = Objects->GetObjectList(ObjectType::Type::Dynamic, ObjectType::Tag::Instancing);
	for (UINT i = 0; i < pList->size(); ++i)
	{
		objectCount += ((InstanceRenderer*)pList->at(i))->GetDrawIntanceCount();
	}
	ImGui::Text("DrawObject : %d", objectCount);
	ImGui::End();
}

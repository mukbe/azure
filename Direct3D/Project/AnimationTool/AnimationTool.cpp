#include "stdafx.h"
#include "AnimationTool.h"

#include "View/FreeCamera.h"
#include "Figure/Figure.h"
#include "CharacterTool.h"
#include "ObjectTool.h"

#include "./Model/ModelAnimPlayer.h"
#include "./Model/Model.h"
#include "./Model/ModelBone.h"
#include "./Model/ModelMesh.h"
#include "./Model/ModelAnimClip.h"

#include "./Project/AnimationTool/FBX/Exporter.h"

#include "./Utilities/String.h"
#include "./Utilities/Path.h"

#include "./View/FreeCamera.h"
#include "./Utilities/Transform.h"
#include "./Figure/Figure.h"

#include "./Environment/Sun.h"
#include "./Renders/DeferredRenderer.h"

AnimationTool::AnimationTool()
	:toolType(0)
{
	AssetManager->LoadAsset();

	RenderRequest->AddRender("AnimationToolUIRender", bind(&AnimationTool::UIRender, this), RenderType::UIRender);
	RenderRequest->AddRender("AnimationToolshadow", bind(&AnimationTool::ShadowRender, this), RenderType::Shadow);
	RenderRequest->AddRender("AnimationToolrender", bind(&AnimationTool::Render, this), RenderType::Render);

	freeCamera = new FreeCamera();

	sun = new Environment::Sun;

	grid = new Figure(Figure::FigureType::Grid, 50.0f, ColorGray(0.4f));
	ObjectManager::Create();
	Objects->SetMainCamera(freeCamera);

	characterTool = new CharacterTool;
	characterTool->SetCamera(freeCamera);

	objectTool = new ObjectTool;
	objectTool->SetCamera(freeCamera);
}


AnimationTool::~AnimationTool()
{
	ObjectManager::Delete();

	SafeDelete(grid);

	SafeDelete(characterTool);
	SafeDelete(objectTool);

	SafeDelete(sun);
	SafeDelete(freeCamera);
}

void AnimationTool::Init()
{
}

void AnimationTool::Release()
{
}

void AnimationTool::PreUpdate()
{
}

void AnimationTool::Update()
{
	if (toolType == 0)
		characterTool->Update();
	else
		objectTool->Update();
}

void AnimationTool::PostUpdate()
{
	freeCamera->Update();
}

void AnimationTool::ShadowRender()
{
	sun->UpdateView();
	sun->Render();

	freeCamera->Render();
	States::SetRasterizer(States::SHADOW);

	States::SetRasterizer(States::SOLID_CULL_ON);

}

void AnimationTool::Render()
{
	freeCamera->Render();

	if (toolType == 0)
	{
		characterTool->Render();
		grid->Render();
	}
	else
		objectTool->Render();

	//camera정보를 deferred에게 언팩킹시에 필요한 정보를 보낸다
	RenderRequest->SetUnPackGBufferProp(freeCamera->GetViewMatrix(), freeCamera->GetProjection());
	sun->Render();

	States::SetSampler(1, States::LINEAR);
}

void AnimationTool::UIRender()
{
	static bool showDemo = false;
	//MainBar
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("ToolList"))
		{
			if (ImGui::MenuItem("CharacterTool"))
				toolType = 0;
			if (ImGui::MenuItem("ObjectTool"))
				toolType = 1;
			if (ImGui::MenuItem("Demo"))
				showDemo = !showDemo;

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}


	if (toolType == 0)
		characterTool->UIRender();
	else
		objectTool->UIRender();

	//Demo
	if (showDemo)
		ImGui::ShowDemoWindow();
}


#include "stdafx.h"
#include "AnimationTool.h"


#include "View/FreeCamera.h"
#include "Figure/Figure.h"
#include "Testing/DirectionalLight.h"
#include "CharacterTool.h"
=======
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
{
	RenderRequest->AddRender("AnimationToolUIRender", bind(&AnimationTool::UIRender, this), RenderType::UIRender);
	RenderRequest->AddRender("AnimationToolshadow", bind(&AnimationTool::ShadowRender, this), RenderType::Shadow);
	RenderRequest->AddRender("AnimationToolrender", bind(&AnimationTool::Render, this), RenderType::Render);


	freeCamera = new FreeCamera();

	characterTool = new CharacterTool;
	characterTool->SetCamera(freeCamera);

	grid = new Figure(Figure::FigureType::Grid, 100.0f, D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f));
	sun = new Environment::Sun;
	debugTransform = new DebugTransform();
	debugTransform->SetCamera(freeCamera);
	debugTransform->ConnectTransform(new Transform);
}


AnimationTool::~AnimationTool()
{
	SafeDelete(characterTool);

	SafeDelete(grid);

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
	characterTool->Update();
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
	grid->ShadowRender();
	States::SetRasterizer(States::SOLID_CULL_ON);

}

void AnimationTool::Render()
{
	freeCamera->Render();
	grid->Render();


	characterTool->Render();

	//camera정보를 deferred에게 언팩킹시에 필요한 정보를 보낸다
	RenderRequest->SetUnPackGBufferProp(freeCamera->GetViewMatrix(), freeCamera->GetProjection());
	sun->Render();

	States::SetSampler(1, States::LINEAR);

}

void AnimationTool::UIRender()
{
	characterTool->UIRender();
}


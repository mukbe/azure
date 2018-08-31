#include "stdafx.h"
#include "AnimationTool.h"

#include "View/FreeCamera.h"
#include "Figure/Figure.h"
#include "Testing/DirectionalLight.h"
#include "CharacterTool.h"
#include "./Renders/DeferredRenderer.h"

AnimationTool::AnimationTool()
{
	RenderRequest->AddRender("AnimationToolUIRender", bind(&AnimationTool::UIRender, this), RenderType::UIRender);
	RenderRequest->AddRender("AnimationToolshadow", bind(&AnimationTool::ShadowRender, this), RenderType::Shadow);
	RenderRequest->AddRender("AnimationToolrender", bind(&AnimationTool::Render, this), RenderType::Render);


	freeCamera = new FreeCamera();
	grid = new Figure(Figure::FigureType::Grid, 100.0f, D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f));
	directionalLight = new DirectionalLight;
	characterTool = new CharacterTool;
	characterTool->SetCamera(freeCamera);
}


AnimationTool::~AnimationTool()
{
	SafeDelete(characterTool);

	SafeDelete(grid);

	SafeDelete(directionalLight);
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
	directionalLight->UpdateView();
	directionalLight->SetBuffer();

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
	DeferredRenderer*deferred = (DeferredRenderer*)RenderRequest->GetDeferred();
	deferred->SetUnPackInfo(freeCamera->GetViewMatrix(), freeCamera->GetProjection());
	directionalLight->SetBuffer();

	States::SetSampler(1, States::LINEAR);

}

void AnimationTool::UIRender()
{
	characterTool->UIRender();
}


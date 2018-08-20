#include "stdafx.h"
#include "Program.h"

#include "./Renders/DeferredRenderer.h"
#include "./Renders/ShadowRenderer.h"
#include "./Project/AnimationTool/AnimationTool.h"

Program::Program()
{
	States::Create();
	
	jsonRoot = new Json::Value();
	JsonHelper::ReadData(L"LevelEditor.json", jsonRoot);

	RenderRequest->AddRenderer("deferred", new DeferredRenderer);
	RenderRequest->AddRenderer("shadow", new ShadowRenderer);
	Scenes->AddScene("anim", new AnimationTool);
}

Program::~Program()
{
	JsonHelper::WriteData(L"LevelEditor.json", jsonRoot);
	SafeDelete(jsonRoot);

	States::Delete();
}

void Program::PreUpdate()
{
	Scenes->PreUpdate();
}

void Program::Update()
{
	Scenes->Update();
}

void Program::PostUpdate()
{
	Scenes->PostUpdate();
}

void Program::Render()
{	
	RenderRequest->Draw();
}

void Program::ResizeScreen()
{
	D3DDesc desc;
	DxRenderer::GetDesc(&desc);
}


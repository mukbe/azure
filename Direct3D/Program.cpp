#include "stdafx.h"
#include "Program.h"

#include "./Renders/DeferredRenderer.h"
#include "./Renders/ShadowRenderer.h"
#include "./Renders/AlphaRenderer.h"
#include "./Renders/RenderTargetBuffer.h"
#include "./Renders/PostEffect/BloomEffect.h"

#include "./Project/AnimationTool/AnimationTool.h"
#include "./Project/TerrainTool/TerrainTool.h"
#include "./Project/ToolScene/ToolScene.h"
#include "./Project/Game/PlayScene.h"

Program::Program()
{
	States::Create();

	jsonRoot = new Json::Value();
	JsonHelper::ReadData(L"LevelEditor.json", jsonRoot);

	AssetManager->LoadAsset();
	
	DeferredRenderer* deferred = new DeferredRenderer;
	RenderRequest->AddRenderer("deferred", deferred);
	RenderRequest->AddRenderer("shadow", new ShadowRenderer);
	AlphaRenderer* alpha = new AlphaRenderer;
	alpha->SetDepthStencilView(deferred->GetDepthStencilView());
	alpha->SetRenderTargetView(pRenderer->GetRenderTargetView());
	RenderRequest->AddRenderer("alpha", alpha);

	BloomEffect* bloomEffect = new BloomEffect;
	bloomEffect->SetDeferredSRV(deferred->GetRenderTargetSRV());
	RenderRequest->AddRenderer("bloom", bloomEffect);

	//Scenes->AddScene("anim", new AnimationTool);
	Scenes->AddScene("tool", new ToolScene);
	//Scenes->AddScene("terrain", new TerrainTool);


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


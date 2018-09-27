#include "stdafx.h"
#include "LoadingScene.h"



LoadingScene::LoadingScene()
{
	
}


LoadingScene::~LoadingScene()
{
}

void LoadingScene::Init()
{
	RenderRequest->AddRender("LoadingUIRender", bind(&LoadingScene::UIRender, this), RenderType::UIRender);
	RenderRequest->AddRender("LoadingRender", bind(&LoadingScene::Render, this), RenderType::Render);
}

void LoadingScene::Release()
{
	loadingFuncList.clear();
	endLoadingFunc = nullptr;
}

void LoadingScene::PreUpdate()
{
}

void LoadingScene::Update()
{
	if (loadingFuncList.empty() == false && succeededCount < loadingFuncList.size())
		loadingFuncList[this->succeededCount]();

	++succeededCount;

	if (this->succeededCount >= loadingFuncList.size())
	{
		if (endLoadingFunc)
			endLoadingFunc();
	}
}

void LoadingScene::PostUpdate()
{
}

void LoadingScene::ShadowRender()
{
}

void LoadingScene::Render()
{
}

void LoadingScene::UIRender()
{
}

void LoadingScene::AddLoadingFunc(function<void()> func)
{
	loadingFuncList.push_back(func);
}

void LoadingScene::SetEndLoadingFunc(function<void()> func)
{
	this->endLoadingFunc = func;
}

#include "stdafx.h"
#include "RenderManager.h"


SingletonCpp(RenderManager)

RenderManager::RenderManager()
{
}


RenderManager::~RenderManager()
{
}

void RenderManager::Draw()
{
	ShadowRender();
	PreRender();
	Render();
	PostRender();
	UIRender();
}

void RenderManager::AddRender(string name, function<void(void)> renderFunc, RenderType type, UINT count)
{
	if (rendering.find(name) == rendering.end())
	{//add
		RenderFunc func;
		func.clear();
		func.push_back(make_pair(count, renderFunc));
		Renders renders;
		renders.insert(make_pair(type, func));
		rendering.insert(make_pair(name, renders));
	}
	else
	{//fail

	}
}

void RenderManager::ShadowRender()
{
	Rendering::iterator renderingIter = rendering.begin();
	for (;renderingIter != rendering.end(); ++renderingIter)
	{
		Renders renders = renderingIter->second;
		RenderFunc funcs = renders[RenderType::Shadow];
		for (size_t i = 0; i < funcs.size(); i++)
		{
			for (size_t j = 0; j < funcs[i].first; j++)
			{
				funcs[i].second();
			}
		}
	}
}

void RenderManager::PreRender()
{
	Rendering::iterator renderingIter = rendering.begin();
	for (;renderingIter != rendering.end(); ++renderingIter)
	{
		Renders renders = renderingIter->second;
		RenderFunc funcs = renders[RenderType::PreRender];
		for (size_t i = 0; i < funcs.size(); i++)
		{
			for (size_t j = 0; j < funcs[i].first; j++)
			{
				funcs[i].second();
			}
		}
	}
}

void RenderManager::Render()
{
	Rendering::iterator renderingIter = rendering.begin();
	for (;renderingIter != rendering.end(); ++renderingIter)
	{
		Renders renders = renderingIter->second;
		RenderFunc funcs = renders[RenderType::Render];
		for (size_t i = 0; i < funcs.size(); i++)
		{
			for (size_t j = 0; j < funcs[i].first; j++)
			{
				funcs[i].second();
			}
		}
	}
}
void RenderManager::PostRender()
{
	Rendering::iterator renderingIter = rendering.begin();
	for (;renderingIter != rendering.end(); ++renderingIter)
	{
		Renders renders = renderingIter->second;
		RenderFunc funcs = renders[RenderType::PostRender];
		for (size_t i = 0; i < funcs.size(); i++)
		{
			for (size_t j = 0; j < funcs[i].first; j++)
			{
				funcs[i].second();
			}
		}
	}
}
void RenderManager::UIRender()
{
	Rendering::iterator renderingIter = rendering.begin();
	for (;renderingIter != rendering.end(); ++renderingIter)
	{
		Renders renders = renderingIter->second;
		RenderFunc funcs = renders[RenderType::UIRender];
		for (size_t i = 0; i < funcs.size(); i++)
		{
			for (size_t j = 0; j < funcs[i].first; j++)
			{
				funcs[i].second();
			}
		}
	}
}

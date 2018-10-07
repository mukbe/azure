#include "stdafx.h"
#include "RenderManager.h"
#include "./Renders/DeferredRenderer.h"

SingletonCpp(RenderManager)

RenderManager::RenderManager()
	:deferred(nullptr),shadow(nullptr),alpha(nullptr), bloomEffect(nullptr)
{
}


RenderManager::~RenderManager()
{
	SafeDelete(deferred);
	SafeDelete(shadow);
	SafeDelete(alpha);
	SafeDelete(bloomEffect);
}

void RenderManager::Draw()
{
	//��ǻ��
	PreRender();

	ShadowRender();

	Render();

	pRenderer->BeginDraw();
	{
		PostRender();
		PostEffect();
		AlphaRender();
		UIRender();
	}
	pRenderer->EndDraw();
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

BOOL RenderManager::EraseRender(string name)
{
	Rendering::iterator Iter = rendering.find(name);
	if (Iter != rendering.end())
	{//ã��
		rendering.erase(Iter);
	}
	else
	{//fail
		return false;
	}
	return true;
}

void RenderManager::AddRenderer(string key, Renderer * renderer)
{
	if (strstr(key.c_str(), "shadow"))
		shadow = renderer;
	if (strstr(key.c_str(), "deferred"))
		deferred = renderer;
	if (strstr(key.c_str(), "alpha"))
		alpha = renderer;
	if (strstr(key.c_str(), "bloom"))
		bloomEffect = renderer;
}

Renderer * RenderManager::FindRenderer(string key)
{
	if (key == "shader")
		return shadow;
	else if (key == "deferred")
		return deferred;
	else if (key == "alpha")
		return alpha;
	else if (key == "bloom")
		return bloomEffect;

	return nullptr;
}

void RenderManager::SetUnPackGBufferProp(D3DXMATRIX view, D3DXMATRIX proj)
{
	((DeferredRenderer*)deferred)->SetUnPackInfo(view, proj);
}

void RenderManager::ShadowRender()
{
	shadow->SetRTV();

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
	//��ǻ��
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
	//gbuffer�ۼ�
	deferred->SetRTV();

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
	//������� ���ε�
	shadow->Render();

	//gbuffer ���ε�, ��ũ���� ���
	deferred->Render();
}

void RenderManager::AlphaRender()
{
	if (alpha)
	{
		alpha->SetRTV();
		alpha->Render();
	}
}

void RenderManager::PostEffect()
{
	if (bloomEffect)
		bloomEffect->Render();
}

void RenderManager::UIRender()
{
	//imgui
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

	ImGui::Render();
}

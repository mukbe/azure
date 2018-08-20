#pragma once

class RenderManager
{
	SingletonHeader(RenderManager)
private:
	enum class RenderType : short
	{
		Shadow, PreRender, Render, PostRender, UIRender
	};
	typedef vector<pair<UINT,function<void(void)>>> RenderFunc;
	typedef map<RenderType, RenderFunc> Renders;
	typedef map<string, Renders> Rendering;
public:
	void Draw();
	void AddRender(string name, function<void(void)> renderFunc, RenderType type, UINT count = 1);

private:
	void ShadowRender();
	void PreRender();
	void Render();
	void PostRender();
	void UIRender();

	Rendering rendering;
};

#define Renderer RenderManager::Get()
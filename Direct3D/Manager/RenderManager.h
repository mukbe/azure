#pragma once

class Renderer
{
public:
	Renderer() {}
	~Renderer() {}

	virtual void Render() = 0;
	virtual void SetRTV() = 0;
	
private:
protected:

};

enum class RenderType : short
{
	PreRender, Shadow, Render,AlphaRender, UIRender
};

class RenderManager
{
	SingletonHeader(RenderManager)
private:
	typedef vector<pair<UINT,function<void(void)>>> RenderFunc;
	typedef map<RenderType, RenderFunc> Renders;
	typedef map<string, Renders> Rendering;
public:
	void Draw();

	void AddRender(string name, function<void(void)> renderFunc, RenderType type, UINT count = 1);
	BOOL EraseRender(string name);

	void AddRenderer(string key, Renderer* renderer);
	Renderer* FindRenderer(string key);
	void SetUnPackGBufferProp(D3DXMATRIX view, D3DXMATRIX proj);
private:
	void ShadowRender();
	void PreRender();
	void Render();
	void PostRender();
	void AlphaRender();
	void PostEffect();
	void UIRender();

	Rendering rendering;

	Renderer* shadow;
	Renderer* deferred;
	Renderer* alpha;
	Renderer* bloomEffect;
};

#define RenderRequest RenderManager::Get()
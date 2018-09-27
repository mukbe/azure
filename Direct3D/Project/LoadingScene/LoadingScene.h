#pragma once
#include "./Manager/SceneNode.h"
class LoadingScene : public SceneNode
{
private:
	UINT succeededCount;
	vector<function<void()>> loadingFuncList;
	function<void()> endLoadingFunc;
public:
	LoadingScene();
	virtual ~LoadingScene();

	void Init();
	void Release();
	void PreUpdate();
	void Update();
	void PostUpdate();
	void ShadowRender();
	void Render();
	void UIRender();

	void AddLoadingFunc(function<void()> func);
	void SetEndLoadingFunc(function<void()> func);
};


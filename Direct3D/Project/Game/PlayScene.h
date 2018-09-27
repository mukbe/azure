#pragma once
#include "./Manager/SceneNode.h"
class PlayScene : public SceneNode
{
public:
	PlayScene();
	virtual ~PlayScene();

	void Init();
	void Release();
	void PreUpdate();
	void Update();
	void PostUpdate();
	void ShadowRender();
	void PreRender();
	void Render();
	void UIRender();

	void InitHungyn();
	void InitSoonwoo();
};


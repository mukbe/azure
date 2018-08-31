#pragma once
#include "./Project/AnimationTool/FBX/Exporter.h"
#include <thread>

class AnimationTool :public SceneNode
{
private:
	class FreeCamera* freeCamera;
	class Figure* box, *grid, *sphere;
	class DirectionalLight* directionalLight;
	class CharacterTool* characterTool;
public:
	AnimationTool();
	virtual ~AnimationTool();
	
	void Init();
	void Release();
	void PreUpdate();
	void Update();
	void PostUpdate();
	void ShadowRender();
	void Render();
	void UIRender();
private:
};


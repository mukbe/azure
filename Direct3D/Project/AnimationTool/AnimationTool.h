#pragma once
#include "./Project/AnimationTool/FBX/Exporter.h"
#include <thread>

namespace Environment
{
	class Sun;
}
class AnimationTool :public SceneNode
{
private:
	class CharacterTool* characterTool;
	class FreeCamera* freeCamera;
	class Figure* grid;
	Environment::Sun* sun;
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


};


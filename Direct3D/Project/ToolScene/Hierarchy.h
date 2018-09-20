#pragma once
#include "ToolBase.h"

class GameObject;
class Hierarchy : public ToolBase
{
private:
	class Inspector* inspector;
private:		//View
	class FreeCamera *freeCamera;
private:		//Enviroment
	class Scattering* scattering;
	class Ocean* ocean;
	Synthesize(GameObject*,targetObject,TargetObject)
public:
	Hierarchy(class ToolScene* toolScene);
	~Hierarchy();

	virtual void Init();
	virtual void Release();
	virtual void PreUpdate();
	virtual void Update();
	virtual void PostUpdate();
	virtual void Render();
	virtual void PreRender();
	virtual void UIRender();
};


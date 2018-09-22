#pragma once
#include "ToolBase.h"
class Inspector : public ToolBase
{
private:
	class Hierarchy* hierarchy;
	class DebugTransform* debugTransform;
	Synthesize(class GameObject*, targetObject,TargetObject)
public:
	Inspector(class ToolScene* toolScene);
	~Inspector();

	virtual void Init();
	virtual void Release();
	virtual void PreUpdate();
	virtual void Update();
	virtual void PostUpdate();
	virtual void Render();
	virtual void PreRender() ;
	virtual void UIRender();
};


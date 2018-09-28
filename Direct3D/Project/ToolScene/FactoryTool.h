#pragma once
#include "ToolBase.h"
class FactoryTool : public ToolBase
{
public:
	FactoryTool(class ToolScene* toolScene);
	~FactoryTool();

	virtual void Init();
	virtual void Release();
	virtual void PreUpdate();
	virtual void Update();
	virtual void PostUpdate();
	virtual void Render();
	virtual void PreRender();
	virtual void UIRender();
};


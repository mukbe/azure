#pragma once
#include "ToolBase.h"
class Picking : public ToolBase
{
private:
	class Inspector* inspector;
public:
	Picking(class ToolScene* toolScene);
	~Picking();

	virtual void Init();
	virtual void Release();
	virtual void PreUpdate();
	virtual void Update();
	virtual void PostUpdate();
	virtual void Render();
	virtual void PreRender();
	virtual void UIRender();
};


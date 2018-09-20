#pragma once

class ToolBase
{
protected:
	class ToolScene* toolScene;
	Synthesize(string,name,Name)
	Synthesize(bool,active,Active)
public:
	ToolBase(class ToolScene* toolScene) :toolScene(toolScene),active(false){}

	virtual void Init() = 0;
	virtual void Release() = 0;
	virtual void PreUpdate() = 0;
	virtual void Update() = 0;
	virtual void PostUpdate() = 0;
	virtual void PreRender() = 0;
	virtual void Render() = 0;
	virtual void UIRender() = 0;
};
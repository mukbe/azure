#pragma once

#include "./Manager/SceneNode.h"
#include <map>
class ToolBase;
class ToolScene : public SceneNode
{
	enum class ToolType{ Hierarchy ,Inspector,Unknown,End};
private:
	multimap<ToolType, ToolBase*> toolList;
	typedef multimap<ToolType, ToolBase*>::iterator ToolIter;
public:
	ToolScene();
	~ToolScene();

	virtual void Init();
	virtual void Release();
	virtual void PreUpdate();
	virtual void Update();
	virtual void PostUpdate();

	void PreRender();
	void Render();
	void UIRender();

	multimap<ToolType, ToolBase*> GetToolList()const { return this->toolList; }
	template<class T>
	T* GetTool(string name);
};

template<class T>
T* ToolScene::GetTool(string name)
{
	{
		ToolIter iter = toolList.begin();
		for (; iter != toolList.end(); ++iter)
		{
			if (iter->second->GetName() == name)
				return reinterpret_cast<T*>(iter->second);
		}

		return nullptr;
	}
}
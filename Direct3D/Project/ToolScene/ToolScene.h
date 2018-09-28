#pragma once

#include "./Manager/SceneNode.h"
#include <map>

#include "./Renders/ShaderBuffer.h"


class ToolBase;
class ToolScene : public SceneNode
{
	enum class ToolType{ Hierarchy = 0 ,Inspector,Factory,Unknown,End};
private:
	unordered_map<ToolType, ToolBase*> toolList;
	typedef unordered_map<ToolType, ToolBase*>::iterator ToolIter;
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

	unordered_map<ToolType, ToolBase*> GetToolList()const { return this->toolList; }
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


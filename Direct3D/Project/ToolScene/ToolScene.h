#pragma once

#include "./Manager/SceneNode.h"
#include <map>

#include "./Renders/ShaderBuffer.h"

class CamBuffer : public ShaderBuffer
{
public:
	struct Struct
	{
		D3DXVECTOR3 pos;
		float padding;
	}data;

	CamBuffer()
		:ShaderBuffer(&data, sizeof Struct)
	{}
};


class ToolBase;
class ToolScene : public SceneNode
{
	enum class ToolType{ Hierarchy = 0 ,Inspector,Unknown,End};
private:
	unordered_map<ToolType, ToolBase*> toolList;
	typedef unordered_map<ToolType, ToolBase*>::iterator ToolIter;
	CamBuffer* camBuffer;
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


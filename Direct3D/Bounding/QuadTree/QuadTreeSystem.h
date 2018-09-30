#pragma once
#include "./Object/GameObject/GameObject.h"
class QuadTreeSystem : public GameObject
{
private:
	class QuadTreeNode* rootNode;
	UINT level;
public:
	QuadTreeSystem();
	~QuadTreeSystem();

	void Update();
	void Render();
	void DebugRender();
	void UIRender();

	void AddObject(class StaticObject* object);
	UINT GetNodeCount();
};


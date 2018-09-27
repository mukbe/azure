#pragma once
#include "./Object/GameObject/GameObject.h"
class StaticObject : public GameObject
{
private:
	Synthesize(class InstanceRenderer*, instanceRenderer,InstanceRenderer)
	Synthesize(vector<class GameCollider*>, colliderList,ColliderList)
public:
	StaticObject(string name);
	virtual ~StaticObject();

	virtual void Release()override;
	virtual void Update()override;
	virtual void DebugRender()override;
	virtual void UIUpdate()override;
	virtual void Render()override;
	virtual void UIRender()override;

	void AddCollider(class GameCollider* collider);
	void AttachQuadTree(class QuadTreeSystem* quadTree);
};


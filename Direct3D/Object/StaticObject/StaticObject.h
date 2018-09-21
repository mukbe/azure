#pragma once
#include "./Object/GameObject/GameObject.h"
class StaticObject : public GameObject
{
private:
	Synthesize(class InstanceRenderer*, instanceRenderer,InstanceRenderer)
	vector<class GameCollider*> colliderList;
public:
	StaticObject(string name);
	virtual ~StaticObject();

	virtual void Release()override;
	virtual void DebugRender()override;
	virtual void UIUpdate()override;
	virtual void UIRender()override;

	void AddCollider(class GameCollider* collider);
};


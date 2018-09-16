#pragma once
#include "./Object/GameObject/GameObject.h"
class StaticObject : public GameObject
{
private:
	vector<class GameCollider*> colliderList;
public:
	StaticObject(string name);
	virtual ~StaticObject();

	virtual void Init()override;
	virtual void Release()override;
	virtual void PreUpdate()override;
	virtual void Update()override;
	virtual void PostUpdate()override;

	void AddCollider(class GameCollider* collider);
};


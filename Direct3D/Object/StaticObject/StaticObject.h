#pragma once
#include "./Object/GameObject/GameObject.h"
class StaticObject : public GameObject
{
private:
	class GameCollider* collider;
public:
	StaticObject(string name);
	virtual ~StaticObject();

	virtual void Init()override;
	virtual void Release()override;
	virtual void PrevUpdate()override;
	virtual void Update()override;
	virtual void PostUpdate()override;
};

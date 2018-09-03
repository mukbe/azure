#pragma once
#include "../GameObject/GameObject.h"
class GameUnit : public GameObject
{
protected:
	class Model* model;
	class ModelAnimPlayer* animation;
	vector<class GameCollider*> colliderList;
public:
	GameUnit(string name,class Model* model);
	~GameUnit();

	virtual void Init();
	virtual void Release();
	virtual void PrevUpdate();
	virtual void Update();
	virtual void PostUpdate();
	virtual void PrevRender();
	virtual void Render();
	virtual void PostRender();
};


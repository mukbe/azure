#pragma once
#include "../GameObject/GameObject.h"
class GameUnit : public GameObject
{
protected:
	class Model* model;
	class ModelAnimPlayer* animation;
	vector<class AnimationCollider*> colliderList;
public:
	GameUnit(string name,class Model* model);
	GameUnit(string name);
	~GameUnit();

	virtual void Init();
	virtual void Release();
	virtual void PreUpdate();
	virtual void Update();
	virtual void PostUpdate();
	virtual void PrevRender();
	virtual void Render();
	virtual void UIRender();

	virtual void SaveData(Json::Value* parent);
	virtual void LoadData(Json::Value* parent);
private:
	void SaveMaterial(wstring file);
};


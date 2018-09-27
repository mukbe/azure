#pragma once
#include "./Object/GameObject/GameObject.h"
class ParticleSystem : public GameObject
{
private:
	
public:
	ParticleSystem();
	~ParticleSystem();

	virtual void Init();
	virtual void Release();
	virtual void PreUpdate();
	virtual void Update();
	virtual void PostUpdate();
	virtual void PrevRender();
	virtual void Render();
	virtual void UIUpdate();
	virtual void UIRender();
	virtual void DebugRender();
};


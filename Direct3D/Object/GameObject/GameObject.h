#pragma once
#include <functional>
#include <map>

class Transform;
class GameObject
{
private:
	typedef function<void(struct TagMessage)>	CallbackFunc;
	typedef map<string, CallbackFunc>			CallbackList;
	typedef map<string, CallbackFunc>::iterator CallbackIter;
protected:
	Synthesize(string, name, Name)
	Synthesize(bool, isActive, isActive)
	Synthesize(bool,isLive,IsLive)
	Synthesize(bool,isRender,IsRender)
	Synthesize(Transform*,transform,Transform)
	CallbackList callbackList;
	vector<struct TagMessage> reserveMessageList;
public:
	bool* GetPIsActive() { return &isActive; }
public:
	GameObject(string name);
	GameObject();
	virtual ~GameObject();

	virtual void Init();
	virtual void Release();
	virtual void PreUpdate();
	virtual void Update();
	virtual void PostUpdate();
	virtual void PrevRender();
	virtual void Render();
	virtual void UIRender();
	virtual void ShadowRender();
	virtual void UIUpdate() {}
	virtual void DebugRender() {}

	void SendMSG(struct TagMessage msg);
	void AddCallback(string name, CallbackFunc func);
	D3DXMATRIX GetFinalMatrix();

	bool IsSame(GameObject* object);
};

/*
string name;
D3DXMATRIX matrix;
*/
#pragma once
#include <functional>
#include <map>

class GameObject
{
private:
	typedef function<void(struct TagMessage)>	CallbackFunc;
	typedef map<string, CallbackFunc>			CallbackList;
	typedef map<string, CallbackFunc>::iterator CallbackIter;
protected:
	Synthesize(string, name, Name)
	Synthesize(bool,isLive,IsLive)
	Synthesize(class Transform*,transform,Transform)
	CallbackList callbackList;
	vector<struct TagMessage> reserveMessageList;
public:
	GameObject(string name);
	virtual ~GameObject();

	virtual void Init();
	virtual void Release();
	virtual void PrevUpdate();
	virtual void Update();
	virtual void PostUpdate();
	virtual void PrevRender();
	virtual void Render();
	virtual void PostRender();

	void SendMSG(struct TagMessage msg);
	void AddCallback(string name, CallbackFunc func);
};

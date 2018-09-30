#include "stdafx.h"
#include "GameObject.h"
#include "Utilities/Transform.h"
#include "TagMessage.h"

GameObject::GameObject(string name)
	:name(name),isLive(true),isRender(true),isActive(true)
{
	transform = new Transform;
}

GameObject::GameObject()
	: name("Unknown"), isLive(true), isRender(true),isActive(true)
{
	transform = new Transform;
}

GameObject::~GameObject()
{
	callbackList.clear();
	reserveMessageList.clear();
	SafeDelete(transform)
}

void GameObject::Init()
{
	
}

void GameObject::Release()
{
	
}

void GameObject::PreUpdate()
{
}

void GameObject::Update()
{
	if (reserveMessageList.empty() == false)
	{
		for (UINT i = 0; i < reserveMessageList.size(); ++i)
		{
			reserveMessageList[i].delayTime -= DeltaTime;

			if (reserveMessageList[i].delayTime < 0.0f)
			{
				CallbackIter iter = callbackList.find(reserveMessageList[i].name);

				if (iter != callbackList.end())
					iter->second(reserveMessageList[i]);

				reserveMessageList.erase(reserveMessageList.begin() + i--);
				Sleep(1);
			}
		}
	}
}

void GameObject::PostUpdate()
{

}

void GameObject::PrevRender()
{
}

void GameObject::Render()
{
}

void GameObject::UIRender()
{
}

void GameObject::ShadowRender()
{
}

void GameObject::SaveData(Json::Value * parent)
{
	Json::Value value;
	{
		JsonHelper::SetValue(value, "Name",this->name);
		string nullString = "";
		JsonHelper::SetValue(value, "FileName", nullString);
		JsonHelper::SetValue(value, "IsActive", isActive);
	}
	(*parent)[this->name.c_str()] = value;
}

void GameObject::LoadData(Json::Value * parent)
{
	JsonHelper::GetValue(*parent, "IsActive", isActive);
}

void GameObject::SendMSG(TagMessage msg)
{
	//딜레이타임이 없다면 바로 메세지 처리
	if (msg.delayTime <= 0.0f)
	{
		CallbackIter iter = callbackList.find(msg.name);
		//찾았다면
		if (iter != callbackList.end())
		{
			TagMessage newMessage = msg;
			iter->second(newMessage);
			return;
		}
	}
	//아닐경우 예약 메세지에 넘긴다
	else
	{
		reserveMessageList.push_back(msg);
	}
}

void GameObject::AddCallback(string name, CallbackFunc func)
{
	pair<string, CallbackFunc> p = make_pair(name, func);
	this->callbackList.insert(p);
}

D3DXMATRIX GameObject::GetFinalMatrix()
{
	return transform->GetFinalMatrix();
}

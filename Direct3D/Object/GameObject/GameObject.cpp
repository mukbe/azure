#include "stdafx.h"
#include "GameObject.h"
#include "Utilities/Transform.h"
#include "TagMessage.h"

GameObject::GameObject(string name)
	:name(name),isLive(true),transform(new Transform)
{
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

void GameObject::PrevUpdate()
{
}

void GameObject::Update()
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

void GameObject::PostUpdate()
{
}

void GameObject::PrevRender()
{
}

void GameObject::Render()
{
}

void GameObject::PostRender()
{
}

void GameObject::SendMSG(TagMessage msg)
{
	//������Ÿ���� ���ٸ� �ٷ� �޼��� ó��
	if (msg.delayTime <= 0.0f)
	{
		CallbackIter iter = callbackList.find(msg.name);
		//ã�Ҵٸ�
		if (iter != callbackList.end())
		{
			iter->second(msg);
			return;
		}
	}
	//�ƴҰ�� ���� �޼����� �ѱ��
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

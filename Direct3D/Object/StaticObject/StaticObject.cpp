#include "stdafx.h"
#include "StaticObject.h"

#include "./Bounding/GameCollider.h"
#include "./Bounding/BoundingBox.h"
#include "./Bounding/BoundingFrustum.h"

#include "./Utilities/Transform.h"
#include "./Object/GameObject/TagMessage.h"

#include "./Renders/Instancing/InstanceRenderer.h"

#include "./View/FreeCamera.h"

StaticObject::StaticObject(string name)
	:GameObject(name), instanceRenderer(nullptr)
{
	this->AddCallback("Delete", [this](TagMessage msg) 
	{
		this->isLive = false;
		TagMessage message;
		message.name = "DeleteObject";
		message.data = &this->name;
		if (instanceRenderer)
			instanceRenderer->SendMSG(message);
	});
}


StaticObject::~StaticObject()
{

}



void StaticObject::Release()
{
	for (UINT i = 0; i < colliderList.size(); ++i)
		SafeDelete(colliderList[i]);
	colliderList.clear();
}

void StaticObject::Update()
{
	if (Scenes->GetIsTool())
	{
		this->isRender = MainCamera->GetFrustum()->IsSphereInFrustum(colliderList[0]);
	}
}

void StaticObject::DebugRender()
{
	for (UINT i = 0; i < colliderList.size(); ++i)
		colliderList[i]->Render(ColorRed,true);
}


void StaticObject::UIUpdate()
{
	for (UINT i = 0; i < colliderList.size(); ++i)
		colliderList[i]->SetFinalMatrix(transform->GetFinalMatrix());
}

void StaticObject::UIRender()
{
	
}

void StaticObject::AddCollider(GameCollider * collider)
{
	GameCollider* newCollider = nullptr;
	collider->Clone((void**)&newCollider);

	newCollider->SetFinalMatrix(newCollider->GetFinalMatrix() * transform->GetFinalMatrix());
	newCollider->SetParentObject(this);
	this->colliderList.push_back(newCollider);
}

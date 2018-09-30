#include "stdafx.h"
#include "StaticObject.h"

#include "./Bounding/GameCollider.h"
#include "./Bounding/BoundingBox.h"
#include "./Bounding/BoundingFrustum.h"

#include "./Utilities/Transform.h"
#include "./Object/GameObject/TagMessage.h"

#include "./Renders/Instancing/InstanceRenderer.h"

#include "./View/FreeCamera.h"

#include "./Bounding/QuadTree/QuadTreeSystem.h"

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
	
}

void StaticObject::DebugRender()
{
	if (this->isRender)
	{
		for (UINT i = 0; i < colliderList.size(); ++i)
			colliderList[i]->Render(ColorRed, true);
	}
}


void StaticObject::UIUpdate()
{
	for (UINT i = 0; i < colliderList.size(); ++i)
		colliderList[i]->Update();
}

void StaticObject::Render()
{
	
}

void StaticObject::UIRender()
{
	
}

void StaticObject::AddCollider(GameCollider * collider)
{
	GameCollider* newCollider = nullptr;
	collider->Clone((void**)&newCollider);

	newCollider->SetFinalMatrix(newCollider->GetLocalMatrix() * transform->GetFinalMatrix());
	newCollider->SetParentObject(this);
	this->colliderList.push_back(newCollider);
}

void StaticObject::AttachQuadTree(QuadTreeSystem * quadTree)
{
	if (quadTree)
		quadTree->AddObject(this);
}

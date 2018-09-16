#include "stdafx.h"
#include "StaticObject.h"

#include "./Bounding/GameCollider.h"
#include "./Bounding/BoundingBox.h"
#include "./Utilities/Transform.h"

StaticObject::StaticObject(string name)
	:GameObject(name)
{

}


StaticObject::~StaticObject()
{

}

void StaticObject::Init()
{
	
}

void StaticObject::Release()
{
	for (UINT i = 0; i < colliderList.size(); ++i)
		SafeDelete(colliderList[i]);
	colliderList.clear();
}

void StaticObject::PreUpdate()
{
}

void StaticObject::Update()
{
}

void StaticObject::PostUpdate()
{
}

void StaticObject::AddCollider(GameCollider * collider)
{
	collider->SetFinalMatrix(collider->GetFinalMatrix() * transform->GetFinalMatrix());
	this->colliderList.push_back(collider);
}

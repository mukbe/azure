#include "stdafx.h"
#include "StaticObject.h"

#include "./Bounding/GameCollider.h"
#include "./Bounding/BoundingBox.h"
#include "./Utilities/Transform.h"

StaticObject::StaticObject(string name)
	:GameObject(name)
{
	this->collider = new GameCollider(this, new BoundingBox);
	this->collider->SetFinalMatrix(this->transform->GetFinalMatrix());
	this->collider->SetType(GameCollider::ColliderType::Static);
}


StaticObject::~StaticObject()
{
	SafeDelete(collider);
}

void StaticObject::Init()
{
}

void StaticObject::Release()
{

}

void StaticObject::PrevUpdate()
{
}

void StaticObject::Update()
{
}

void StaticObject::PostUpdate()
{
}

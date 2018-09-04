#include "stdafx.h"
#include "GameCollider.h"
#include "BoundingBox.h"

#include "../Object/GameObject/GameObject.h"
#include "AnimationCollider.h"
#include "../Utilities/BinaryFile.h"

GameCollider::GameCollider(GameObject* parentObject,BoundingBox* bounding)
	:parentObject(parentObject),boundingBox(bounding),name("Collider"),type(UnKnown)
{
}


GameCollider::~GameCollider()
{
	SafeDelete(boundingBox);
}

void GameCollider::Update()
{
	
}

void GameCollider::Render(D3DXCOLOR color)
{
	pRenderer->ChangeZBuffer(false);
	vector<D3DXVECTOR3> corners;
	this->boundingBox->GetCorners(corners, this->finalMatrix);
	GizmoRenderer->OBB(corners, color);
	pRenderer->ChangeZBuffer(true);
}

bool GameCollider::IsIntersect(GameCollider * collider)
{
	return BoundingBox::IntersectsOBB(this->finalMatrix, this->boundingBox, collider->GetFinalMatrix(), collider->boundingBox);
}

string GameCollider::GetTypeName(int index)
{
	if (index == 0)
		return "RigidBody";
	else if (index == 1)
		return "Attack";
	else if (index == 2)
		return "HeatBox";
	return string();
}

void GameCollider::SaveCollider(BinaryWriter * w, AnimationCollider * collider)
{
	w->String(collider->name);
	w->Int(collider->type);
	w->Int(collider->GetParentBoneIndex());
	w->Byte(collider->GetlocalMatrix(),sizeof D3DXMATRIX);
}

void GameCollider::LoadCollider(BinaryReader * r, AnimationCollider * collider)
{
	collider->name = r->String();
	collider->type = (ColliderType)r->Int();
	collider->SetParentBoneIndex(r->Int());
	D3DXMATRIX matLocal;
	void* ptr = &matLocal;
	r->Byte(&ptr, sizeof D3DXMATRIX);
	collider->SetlocalMatrix(matLocal);
}

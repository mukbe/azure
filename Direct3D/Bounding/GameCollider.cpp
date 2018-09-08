#include "stdafx.h"
#include "GameCollider.h"
#include "BoundingBox.h"

#include "../Object/GameObject/GameObject.h"
#include "AnimationCollider.h"
#include "../Utilities/BinaryFile.h"

GameCollider::GameCollider(GameObject* parentObject,BoundingBox* bounding)
	:parentObject(parentObject),boundingBox(bounding),name("Collider"),type(UnKnown)
{
	D3DXMatrixIdentity(&finalMatrix);
}


GameCollider::~GameCollider()
{
	SafeDelete(boundingBox);
}

void GameCollider::Update()
{
	
}

void GameCollider::Render(D3DXCOLOR color,bool bZbufferOn)
{
	if(bZbufferOn == false)
		pRenderer->ChangeZBuffer(false);
	{
		vector<D3DXVECTOR3> corners;
		this->boundingBox->GetCorners(corners, this->finalMatrix);
		GizmoRenderer->OBB(corners, color);
	}
	if (bZbufferOn == false)
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
	else if (index == 3)
		return "Static";
	return string();
}

void GameCollider::SaveAnimCollider(BinaryWriter * w, AnimationCollider * collider)
{
	w->String(collider->name);
	w->Int(collider->type);
	w->Int(collider->GetParentBoneIndex());
	w->Byte(collider->GetlocalMatrix(),sizeof D3DXMATRIX);
}

void GameCollider::LoadAnimCollider(BinaryReader * r, AnimationCollider * collider)
{
	collider->name = r->String();
	collider->type = (ColliderType)r->Int();
	collider->SetParentBoneIndex(r->Int());
	D3DXMATRIX matLocal;
	void* ptr = &matLocal;
	r->Byte(&ptr, sizeof D3DXMATRIX);
	collider->SetlocalMatrix(matLocal);
}

void GameCollider::SaveCollider(BinaryWriter * w, GameCollider * collider)
{
	w->String(collider->name);
	w->Int(collider->type);
	w->Byte(collider->GetFinalMatrix(), sizeof D3DXMATRIX);

	collider->boundingBox->SaveData(w);
}

void GameCollider::LoadCollider(BinaryReader * r, GameCollider * collider)
{
	collider->name = r->String();
	collider->type = (ColliderType)r->Int();
	D3DXMATRIX matFinal;
	void* ptr = &matFinal;
	r->Byte(&ptr, sizeof D3DXMATRIX);
	collider->SetFinalMatrix(matFinal);

	collider->boundingBox->LoadData(r);
}

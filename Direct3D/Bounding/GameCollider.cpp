#include "stdafx.h"
#include "GameCollider.h"
#include "BoundingBox.h"

#include "../Object/GameObject/GameObject.h"
#include "AnimationCollider.h"
#include "../Utilities/BinaryFile.h"

#include "BoundingSphere.h"

GameCollider::GameCollider(GameObject* parentObject,BoundingBox* bounding)
	:parentObject(parentObject),boundingBox(bounding),name("Collider"),type(UnKnown)
{
	D3DXMatrixIdentity(&finalMatrix);
	D3DXMatrixIdentity(&localMatrix);
}


GameCollider::~GameCollider()
{
	SafeDelete(boundingBox);
}

void GameCollider::Clone(void ** clone)
{
	BoundingBox* box = new BoundingBox;
	box->minPos = this->boundingBox->minPos;
	box->maxPos = this->boundingBox->maxPos;
	box->halfSize = this->boundingBox->halfSize;

	GameCollider* collider = new GameCollider(nullptr,box);
	collider->name = this->name;
	collider->type = this->type;
	collider->finalMatrix = this->finalMatrix;
	collider->localMatrix = this->localMatrix;

	*clone = collider;
}

void GameCollider::Update()
{
	if (parentObject)
		D3DXMatrixMultiply(&finalMatrix, &localMatrix, &parentObject->GetFinalMatrix());
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

bool GameCollider::IsIntersectSphere(GameCollider * box)
{
	D3DXVECTOR3 worldPosA,worldPosB;
	float worldRadiusA,worldRadiusB;
	this->GetWorldCenterRadius(&worldPosA, &worldRadiusA);
	box->GetWorldCenterRadius(&worldPosB, &worldRadiusB);

	float result = D3DXVec3LengthSq(&(worldPosA - worldPosB));

	if ((double)worldRadiusA * (double)worldRadiusA + 2.0 * (double)worldRadiusA * (double)worldRadiusB
		+ (double)worldRadiusB * (double)worldRadiusB <= (double)result)
		return false;
	else
		return true;
	
	return false;
}

void GameCollider::GetCorners(vector<D3DXVECTOR3>& output)
{
	this->boundingBox->GetCorners(output, this->finalMatrix);
}

void GameCollider::GetWorldCenterRadius(D3DXVECTOR3 * pOutCenter, float * pOutRadius)
{
	this->boundingBox->GetWorldCenterAndRadius(pOutCenter, pOutRadius,this->finalMatrix);
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
	w->Byte(collider->GetLocalMatrix(),sizeof D3DXMATRIX);
}

void GameCollider::LoadAnimCollider(BinaryReader * r, AnimationCollider * collider)
{
	collider->name = r->String();
	collider->type = (ColliderType)r->Int();
	collider->SetParentBoneIndex(r->Int());
	D3DXMATRIX matLocal;
	void* ptr = &matLocal;
	r->Byte(&ptr, sizeof D3DXMATRIX);
	collider->SetLocalMatrix(matLocal);
}

void GameCollider::SaveCollider(BinaryWriter * w, GameCollider * collider)
{
	w->String(collider->name);
	w->Int(collider->type);
	w->Byte(collider->GetFinalMatrix(), sizeof D3DXMATRIX);
	w->Byte(collider->GetLocalMatrix(), sizeof D3DXMATRIX);

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

	D3DXMATRIX matLocal;
	void* ptr2 = &matLocal;
	r->Byte(&ptr2, sizeof D3DXMATRIX);
	collider->SetLocalMatrix(matFinal);

	collider->boundingBox->LoadData(r);
}

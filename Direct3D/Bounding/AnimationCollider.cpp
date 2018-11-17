#include "stdafx.h"
#include "AnimationCollider.h"

#include "../Object/GameObject/GameObject.h"
#include "../Model/ModelAnimPlayer.h"
#include "BoundingBox.h"
AnimationCollider::AnimationCollider(GameObject* parentObject,ModelAnimPlayer* animation)
	:GameCollider(parentObject,new BoundingBox(D3DXVECTOR3(-0.5f,-0.5f,-0.5f),D3DXVECTOR3(0.5f,0.5f,0.5f))),parentBoneIndex(-1),animation(animation)
{
	this->type = GameCollider::ColliderType::HeatBox;
	D3DXMatrixIdentity(&localMatrix);
}


AnimationCollider::~AnimationCollider()
{
}

void AnimationCollider::Clone(void ** clone)
{
	BoundingBox* box = new BoundingBox;
	box->minPos = this->boundingBox->minPos;
	box->maxPos = this->boundingBox->maxPos;
	box->halfSize = this->boundingBox->halfSize;

	AnimationCollider* collider = new AnimationCollider(nullptr, nullptr);
	collider->name = this->name;
	collider->type = this->type;
	collider->finalMatrix = this->finalMatrix;
	collider->localMatrix = this->localMatrix;
	collider->parentBoneIndex = this->parentBoneIndex;
	
	*clone = collider;
}

void AnimationCollider::Update()
{
	D3DXMatrixIdentity(&this->finalMatrix);
	if (animation)
	{
		if (parentBoneIndex >= 0)
		{
			D3DXMatrixMultiply(&finalMatrix, &this->localMatrix, &animation->GetBoneAnimation(parentBoneIndex));
		}
	}
}

void AnimationCollider::DebugUpdate(D3DXMATRIX matLocal, D3DXMATRIX matFinal)
{
	this->finalMatrix = matFinal;
	this->localMatrix = matLocal;
}

void AnimationCollider::Render(D3DXCOLOR color,bool b)
{
	GameCollider::Render(color,b);
}


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

void AnimationCollider::Render()
{
	GameCollider::Render();
}

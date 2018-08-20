#include "stdafx.h"
#include "ModelAnimPlayer.h"

#include "Model.h"
#include "ModelBone.h"
#include "ModelMesh.h"
#include "ModelAnimClip.h"
#include "../Renders/Material.h"
#include "../Utilities/Transform.h"


void ModelAnimPlayer::ChangeAnimation(wstring animName, float blendTime)
{
	this->endBlendClip = model->Clip(animName);
	if (endBlendClip == NULL)return;

	this->blendStartKeyframe = this->currentKeyframe;
	this->blendFrameTime = blendTimeFactor = 0.0f;
	this->totalBlendingTime = blendTime;
	this->playState = PlayState::Blending;
	this->endClipName = animName;
	this->endClipInex = -1;
}

void ModelAnimPlayer::ChangeAnimation(UINT index, float blendTime)
{
	this->endBlendClip = model->Clip(index);
	if (endBlendClip == NULL)return;

	this->blendStartKeyframe = this->currentKeyframe;
	this->blendFrameTime = blendTimeFactor = 0.0f;
	this->totalBlendingTime = blendTime;
	this->playState = PlayState::Blending;
	this->endClipInex = index;
	this->endClipName = L"";
}

void ModelAnimPlayer::UpdateAnimationByBlending()
{
	this->UpdateTimeByBlending();
	
	if (this->playState == PlayState::Normal)
	{
		this->UpdateBone();
		return;
	}
	
	this->UpdateBoneByBlending();
}


void ModelAnimPlayer::UpdateTimeByBlending()
{
	this->blendFrameTime += DeltaTime;

	float invFrameRate = 1.0f / this->totalBlendingTime;
	if (this->blendFrameTime > totalBlendingTime)
	{
		if (endClipName.length() > 0)
			this->ChangeAnimation(endClipName);
		else if(endClipInex >= 0)
			this->ChangeAnimation(endClipInex);
	}

	this->blendTimeFactor = blendFrameTime / totalBlendingTime;
}

void ModelAnimPlayer::UpdateBoneByBlending()
{
	int index = 0;
	for (UINT i = 0; i < model->BoneCount(); i++)
	{
		ModelBone* bone = model->Bone(i);

		D3DXMATRIX matAnimation;
		D3DXMATRIX matParentAnimation;

		D3DXMATRIX matInvBindpose = bone->AbsoluteTransform();
		D3DXMatrixInverse(&matInvBindpose, NULL, &matInvBindpose);

		ModelKeyframe* startFrame = currentClip->Keyframe(bone->Name());
		if (startFrame == nullptr)continue;
		ModelKeyframe* endFrame = endBlendClip->Keyframe(bone->Name());
		if (endFrame == nullptr)continue;

		D3DXMATRIX S, R, T;

		ModelKeyframeData current = startFrame->Datas[blendStartKeyframe];
		ModelKeyframeData next = endFrame->Datas[0];

		//스케일 보간
		D3DXVECTOR3 s1 = current.Scale;
		D3DXVECTOR3 s2 = next.Scale;

		D3DXVECTOR3 s;
		D3DXVec3Lerp(&s, &s1, &s2, blendTimeFactor);
		D3DXMatrixScaling(&S, s.x, s.y, s.z);

		//회전 보간
		D3DXQUATERNION q1 = current.Rotation;
		D3DXQUATERNION q2 = next.Rotation;

		D3DXQUATERNION q;
		D3DXQuaternionSlerp(&q, &q1, &q2, blendTimeFactor);
		D3DXMatrixRotationQuaternion(&R, &q);

		//위치 보간
		D3DXVECTOR3 t1 = current.Translation;
		D3DXVECTOR3 t2 = next.Translation;

		D3DXVECTOR3 t;
		D3DXVec3Lerp(&t, &t1, &t2, blendTimeFactor);
		D3DXMatrixTranslation(&T, t.x, t.y, t.z);

		matAnimation = S * R * T;

		int parentIndex = bone->ParentIndex();
		if (parentIndex < 0)
			D3DXMatrixIdentity(&matParentAnimation);
		else
			matParentAnimation = boneAnimation[parentIndex];

		boneAnimation[index] = matAnimation * matParentAnimation;
		skinTransform[index] = matInvBindpose * boneAnimation[index];

		++index;

	}//for(model->BoneCount())
}
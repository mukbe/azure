#include "stdafx.h"
#include "ModelAnimPlayer.h"

#include "Model.h"
#include "ModelBone.h"
#include "ModelMesh.h"
#include "ModelAnimClip.h"
#include "../Renders/Material.h"
#include "../Utilities/Transform.h"

ModelAnimPlayer::ModelAnimPlayer(Model * model)
	: model(model), currentClip(nullptr), mode(Mode::Play),
	currentKeyframe(0), frameTime(0.0f), keyframeFactor(0.0f),
	nextKeyframe(0), useQuaternionKeyframe(true), playState(PlayState::Normal),
	blendFrameTime(0.f),totalBlendingTime(0.f),blendTimeFactor(0.f),blendStartKeyframe(0)
{
	//TODO 모델 세이더 추가 시에 작업
	shader = Shaders->FindShader("modelShader");
	//shadowShader = new Shader(L"");

	if (model == nullptr)return;
	currentClip = model->Clip(0);
	this->Play();

}

ModelAnimPlayer::~ModelAnimPlayer()
{
	model = nullptr;

	this->skinTransform.clear();
	this->boneAnimation.clear();
}

void ModelAnimPlayer::Init()
{
	this->model = nullptr;
	this->currentClip = nullptr;
	this->mode = Mode::Stop;
	this->currentKeyframe = 0.f;
	this->frameTime = 0.f;
	this->keyframeFactor = 0.f;
	this->nextKeyframe = this->currentKeyframe = 0;
	this->useQuaternionKeyframe = true;
	this->playState = PlayState::Normal;
}

void ModelAnimPlayer::Update()
{
	if (model == nullptr)return;
	if (currentClip == nullptr || mode != Mode::Play)
		return;

	if (this->playState == PlayState::Normal)
		UpdateAnimation();
	else if (this->playState == PlayState::Blending)
		UpdateAnimationByBlending();

	model->Buffer()->SetBoneTransforms(skinTransform.data(), skinTransform.size());
}



void ModelAnimPlayer::Render()
{
	if (model == nullptr)return;
	model->Buffer()->SetVSBuffer(6);

	for (ModelMesh* mesh : model->Meshes())
		mesh->Render(this->shader);
}


void ModelAnimPlayer::SetKeyframe(int key)
{
	frameTime = 0.0f;
	currentKeyframe = key;
	nextKeyframe = currentKeyframe + 1;
}

void ModelAnimPlayer::UpdateTime()
{
	frameTime += DeltaTime;

	//1프레임 당 소요시간.
	float invFrameRate = 1.0f / currentClip->FrameRate();
	while (frameTime > invFrameRate)
	{
		int keyframeCount = (int)currentClip->TotalFrame();
		currentKeyframe = (currentKeyframe + 1) % keyframeCount;
		nextKeyframe = (currentKeyframe + 1) % keyframeCount;


		frameTime -= invFrameRate;
	}

	keyframeFactor = frameTime / invFrameRate;

}

void ModelAnimPlayer::UpdateBone()
{
	if (skinTransform.size() < 1)
		skinTransform.assign(model->BoneCount(), D3DXMATRIX());

	if (boneAnimation.size() < 1)
		boneAnimation.assign(model->BoneCount(), D3DXMATRIX());

	int index = 0;
	for (UINT i = 0; i < model->BoneCount(); i++)
	{
		ModelBone* bone = model->Bone(i);

		D3DXMATRIX matAnimation;
		D3DXMATRIX matParentAnimation;

		D3DXMATRIX matInvBindpose = bone->AbsoluteTransform();
		D3DXMatrixInverse(&matInvBindpose, NULL, &matInvBindpose);

		ModelKeyframe* frame = currentClip->Keyframe(bone->Name());
		if (frame == nullptr)continue;

		if (useQuaternionKeyframe == true)
		{
			D3DXMATRIX S, R, T;

			ModelKeyframeData current = frame->Datas[currentKeyframe];
			ModelKeyframeData next = frame->Datas[nextKeyframe];

			//스케일 보간
			D3DXVECTOR3 s1 = current.Scale;
			D3DXVECTOR3 s2 = next.Scale;

			D3DXVECTOR3 s;
			D3DXVec3Lerp(&s, &s1, &s2, keyframeFactor);
			D3DXMatrixScaling(&S, s.x, s.y, s.z);

			//회전 보간
			D3DXQUATERNION q1 = current.Rotation;
			D3DXQUATERNION q2 = next.Rotation;

			D3DXQUATERNION q;
			D3DXQuaternionSlerp(&q, &q1, &q2, keyframeFactor);
			D3DXMatrixRotationQuaternion(&R, &q);

			//위치 보간
			D3DXVECTOR3 t1 = current.Translation;
			D3DXVECTOR3 t2 = next.Translation;

			D3DXVECTOR3 t;
			D3DXVec3Lerp(&t, &t1, &t2, keyframeFactor);
			D3DXMatrixTranslation(&T, t.x, t.y, t.z);

			matAnimation = S * R * T;
		}
		else
		{
			matAnimation = frame->Datas[currentKeyframe].Transform;
		}

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



void ModelAnimPlayer::UpdateAnimation()
{
	UpdateTime();
	UpdateBone();
}


void ModelAnimPlayer::Play()
{
	mode = Mode::Play;
}

void ModelAnimPlayer::Pause()
{
	mode = Mode::Pause;
}

void ModelAnimPlayer::Stop()
{
	mode = Mode::Stop;

	frameTime = 0.f;
	keyframeFactor = 0.f;
	currentKeyframe = 0;
	nextKeyframe = 1;
}

void ModelAnimPlayer::ChangeAnimation(wstring animName, function<void()> func)
{
	this->playState = PlayState::Normal;
	currentClip = model->Clip(animName);
	this->Stop();
	this->Play();
}

void ModelAnimPlayer::ChangeAnimation(UINT index)
{
	this->playState = PlayState::Normal;
	currentClip = model->Clip(index);
	this->Stop();
	this->Play();
}

void ModelAnimPlayer::TPose()
{
	this->currentClip = nullptr;
	this->mode = Mode::Stop;
	this->playState = PlayState::Normal;
	this->currentKeyframe = 0.f;
	this->frameTime = 0.f;
	this->keyframeFactor = 0.f;
	this->nextKeyframe = this->currentKeyframe = 0;
}
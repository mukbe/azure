#include "stdafx.h"
#include "ComputeAnimation.h"

#include "./ComputeShader/ComputeShader.h"

#include "./Model/Model.h"
#include "./Model/ModelAnimClip.h"
#include "./Model/ModelBone.h"
#include "./Model/ModelMesh.h"

ComputeAnimation::ComputeAnimation()
	:currentKeyFrame(0),nextKeyFrame(0),frameTime(0.0f),keyFrameFactor(0.0f), transform(nullptr), model(nullptr)
{
	computeKeyFrameShader = new ComputeShader(ShaderPath + L"009_ComputeAnimation.hlsl","ComputeKeyFrame");
	computeAnimationShader = new ComputeShader(ShaderPath + L"009_ComputeAnimation.hlsl", "ComputeAnimation");
	animationBuffer = new ComputeAnimationBuffer;
	//shader = new Shader(ShaderPath + L"010_DeferredAnimation.hlsl");
	shader = Shaders->FindShader("modelShader");
	worldBuffer = Buffers->FindShaderBuffer<WorldBuffer>();
}


ComputeAnimation::~ComputeAnimation()
{
	SafeDelete(model);
	SafeDelete(keyFrameBuffer);
	SafeDelete(boneDataBuffer);

	clips.clear();
	skinAnimations.clear();
	boneAnimations.clear();

	//SafeDelete(shader);
	SafeDelete(computeAnimationShader);
	SafeDelete(computeKeyFrameShader);
	SafeDelete(animationBuffer);
	SafeDelete(skinAnimationBuffer);
	SafeDelete(boneAnimationBuffer)
	SafeDelete(computingKeyFrameBuffer);
}

void ComputeAnimation::Update()
{
	this->UpdateFrame();
	this->UpdateBuffer();
	this->ComputingAnimation();

	computingKeyFrameBuffer->GetDatas(testList.data());
	
	int index = 0;
	for (UINT i = 0; i < model->BoneCount(); i++)
	{
		ModelBone* bone = model->Bone(i);
	
		D3DXMATRIX matAnimation = testList[i];
		D3DXMATRIX matParentAnimation;
	
		D3DXMATRIX matInvBindpose = bone->AbsoluteTransform();
		D3DXMatrixInverse(&matInvBindpose, NULL, &matInvBindpose);
	
		int parentIndex = bone->ParentIndex();
		if (parentIndex < 0)
		{
			if (transform)
				matParentAnimation = transform->GetFinalMatrix();
			else
				D3DXMatrixIdentity(&matParentAnimation);
		}
		else
			matParentAnimation = boneAnimations[parentIndex];
	
		D3DXMatrixMultiply(&boneAnimations[index], &matAnimation, &matParentAnimation);
		D3DXMatrixMultiply(&skinAnimations[index], &matInvBindpose, &boneAnimations[index]);
	
		++index;
	}//for(model->BoneCount())

}

void ComputeAnimation::Render()
{
	if (model == nullptr)return;

	ID3D11ShaderResourceView* srv = skinAnimationBuffer->GetSRV();
	DeviceContext->VSSetShaderResources(10, 1, &srv);

	model->Buffer()->SetBoneTransforms(skinAnimations.data(), skinAnimations.size());
	model->Buffer()->SetVSBuffer(6);

	for (ModelMesh* mesh : model->Meshes())
		mesh->Render(this->shader);
}


void ComputeAnimation::UpdateFrame()
{
	frameTime += DeltaTime;

	float invFrameRate = 1.0f / currentClip.frameRate;
	while (frameTime > invFrameRate)
	{
		int keyframeCount = (int)currentClip.totalFrame;
		currentKeyFrame = (currentKeyFrame + 1) % keyframeCount;
		nextKeyFrame = (currentKeyFrame + 1) % keyframeCount;
	
		frameTime -= invFrameRate;
	}
	
	keyFrameFactor = frameTime / invFrameRate;
}

void ComputeAnimation::UpdateBuffer()
{
	animationBuffer->data.keyFrame = currentKeyFrame;
	animationBuffer->data.nextKeyFrame = nextKeyFrame;
	animationBuffer->data.frameFactor = keyFrameFactor;
	
	animationBuffer->data.currentClipOffset = currentClip.clipOffset;
	animationBuffer->data.currentClipTotalFrame = currentClip.totalFrame;

	if(transform)
		this->worldBuffer->SetMatrix(transform->GetFinalMatrix());
}

void ComputeAnimation::ComputingAnimation()
{
	//BindingBuffer------------------------------------
	//-------------------------------------------------
	animationBuffer->SetCSBuffer(10);
	worldBuffer->SetCSBuffer(1);

	keyFrameBuffer->BindResource(0);
	boneDataBuffer->BindResource(1);
	skinAnimationBuffer->BindResource(2);
	boneAnimationBuffer->BindResource(3);
	computingKeyFrameBuffer->BindResource(4);
	//Computing -----------------------------------------
	//--------------------------------------------------
	computeKeyFrameShader->BindShader();
	computeKeyFrameShader->Dispatch(1, 1, 1);

	//computeAnimationShader->BindShader();
	//computeAnimationShader->Dispatch(1, 1, 1);
	//UnbindBuffer--------------------------------------
	//-------------------------------------------------

	keyFrameBuffer->ReleaseResource(0);
	boneDataBuffer->ReleaseResource(1);
	skinAnimationBuffer->ReleaseResource(2);
	boneAnimationBuffer->ReleaseResource(3);
	computingKeyFrameBuffer->ReleaseResource(4);

}

void ComputeAnimation::BuildBuffer(Model * model)
{
	D3DXMATRIX iden;
	D3DXMatrixIdentity(&iden);
	this->model = model;

	UINT boneCount = model->BoneCount();
	skinAnimations.assign(boneCount, iden);
	boneAnimations.assign(boneCount, iden);
	testList.assign(boneCount, iden);
	animationBuffer->data.boneCount = boneCount;

	struct BoneData { D3DXMATRIX mat; int parentBoneIndex; };

	vector<ModelBone*> bones = model->Bones();
	vector<BoneData> bonesInvBindPoses;

	for (size_t i = 0; i < boneCount; ++i)
	{
		BoneData data;
		data.parentBoneIndex = model->Bone(i)->ParentIndex();
		data.mat = model->Bone(i)->AbsoluteTransform();
		D3DXMatrixInverse(&data.mat, nullptr, &data.mat);
		bonesInvBindPoses.push_back(data);
	}

	//현재 본의 키프레임 인덱스 = clipOffset + (본인덱스 * 현재클립의 totalFrame) + currentKeyFrame;

	int clipOffset = 0;
	vector<ModelKeyframeData> keyFrames;
	vector<ModelAnimClip*> modelClips = model->Clips();
	for (UINT i = 0; i < modelClips.size(); ++i)
	{
		ComputeAnimationClip computeClip;
		computeClip.name = modelClips[i]->Name();
		computeClip.totalFrame = modelClips[i]->TotalFrame();
		computeClip.frameRate = modelClips[i]->FrameRate();
		computeClip.clipOffset = clipOffset;
		clips.insert(make_pair(computeClip.name, computeClip));

		unordered_map<wstring, ModelKeyframe*> keyFramemap = modelClips[i]->KeyFrameMap();
		for (UINT k = 0; k < boneCount; ++k)
		{
			vector<ModelKeyframeData> currentData = keyFramemap[bones[k]->Name()]->Datas;
			for (UINT j = 0; j < currentData.size(); ++j)
			{
				keyFrames.push_back(currentData[j]);
			}
		}

		clipOffset += model->BoneCount() * modelClips[i]->TotalFrame();
		keyFramemap.clear();
	}

	vector<D3DXMATRIX> dummy2;
	dummy2.assign(boneCount, iden);
	
	//CreateBuffers
	keyFrameBuffer = new CResource1D(sizeof ModelKeyframeData, keyFrames.size(), keyFrames.data());
	boneDataBuffer = new CResource1D(sizeof BoneData, bonesInvBindPoses.size(), bonesInvBindPoses.data());
	skinAnimationBuffer = new CResource1D(sizeof D3DXMATRIX, boneCount, dummy2.data());
	boneAnimationBuffer = new CResource1D(sizeof D3DXMATRIX, boneCount, dummy2.data());
	computingKeyFrameBuffer = new CResource1D(sizeof D3DXMATRIX, boneCount, dummy2.data());

	//ReleaseStackMemory
	dummy2.clear();
	bonesInvBindPoses.clear();
	bones.clear();
	modelClips.clear();
	keyFrames.clear();
}

void ComputeAnimation::ChangeAnimation(wstring name)
{
	ClipIter iter = clips.find(name);
	if (iter != clips.end())
	{
		this->currentClip = iter->second;
		this->frameTime = 0.0f;
		this->currentKeyFrame = this->nextKeyFrame = 0;
	}
}

void ComputeAnimation::ChangeAnimation(UINT i)
{
	ClipIter iter = clips.begin();
	UINT count = 0;
	for (; iter != clips.end(); ++iter, ++count)
	{
		if (count == i)
		{
			this->currentClip = iter->second;
			break;
		}
	}

}

#pragma once
#include "Renders/ShaderBuffer.h"

/***********************************
현재 본의 키프레임 인덱스 = clipOffset + (본인덱스 * 현재클립의 totalFrame) + currentKeyFrame;
***********************************/

class Model;
class ModelAnimClip;
class ModelBone;

class ComputeAnimationBuffer : public ShaderBuffer
{
public:
	struct Struct {
		float frameFactor;
		int keyFrame;
		int nextKeyFrame;
		int currentClipOffset;
		int currentClipTotalFrame;
		int boneCount;
		D3DXVECTOR2 padding;
	}data;

	ComputeAnimationBuffer()
		:ShaderBuffer(&data, sizeof Struct) {

	};
};

struct ComputeAnimationClip
{
	wstring name;
	int totalFrame;
	float frameRate;
	int clipOffset;			//현재 클립의 indexOffset값 	
};


class ComputeAnimation
{
private:
	typedef unordered_map<wstring, struct ComputeAnimationClip>::iterator ClipIter;
private:
	class Shader* shader;
	class ComputeShader* computeKeyFrameShader;
	class ComputeShader* computeAnimationShader;
	class Transform* transform;
	class Model* model;

	vector<D3DXMATRIX> skinAnimations;
	vector<D3DXMATRIX> boneAnimations;
	vector<D3DXMATRIX> testList;
	unordered_map<wstring,struct ComputeAnimationClip> clips;
	ComputeAnimationClip currentClip;

	ComputeAnimationBuffer* animationBuffer;
	class CResource1D* keyFrameBuffer;
	class CResource1D* boneDataBuffer;
	class CResource1D* skinAnimationBuffer;
	class CResource1D* boneAnimationBuffer;
	class CResource1D* computingKeyFrameBuffer;

	class WorldBuffer* worldBuffer;

	int currentKeyFrame;
	int nextKeyFrame;
	float frameTime;
	float keyFrameFactor;
public:
	ComputeAnimation();
	~ComputeAnimation();

	void Update();
	void Render();

	void BuildBuffer(class Model* model);
	void BindTransform(class Transform* transform) { this->transform = transform; }
	void ChangeAnimation(wstring name);
	void ChangeAnimation(UINT i);
private:
	void UpdateFrame();
	void UpdateBuffer();
	void ComputingAnimation();
};

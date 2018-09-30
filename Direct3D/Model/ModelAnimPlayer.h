#pragma once
#include <functional>

class Model;
class ModelAnimClip;
class ModelBone;

class ModelAnimPlayer //Animation Controller
{
public:
	enum class Mode
	{
		Play = 0, Pause, Stop
	};
	enum class PlayState
	{
		Normal,Blending,End
	};

	ModelAnimPlayer(Model* model);
	~ModelAnimPlayer();

	void Init();

	void Update();
	void Render();

	void Play();
	void Pause();
	void Stop();

	//NoBlending
	void ChangeAnimation(wstring animName, function<void()> func = NULL);
	void ChangeAnimation(UINT index);
	//UseBlending
	void ChangeAnimation(wstring animName, float blendTime);
	void ChangeAnimation(UINT index, float blendTime);
	
	void TPose();

	Model* GetModel()const { return this->model; }

	vector<D3DXMATRIX> GetBoneAnimations()const { return this->boneAnimation; }
	D3DXMATRIX GetBoneAnimation(UINT index) { return this->boneAnimation[index]; }
	void SetKeyframe(int key);
private:
	void UpdateAnimation();
	void UpdateAnimationByBlending();

	void UpdateTime();
	void UpdateBone();

	void UpdateTimeByBlending();
	void UpdateBoneByBlending();
private:
	Synthesize(Shader *, shader,MainShader)
	Synthesize(Shader*, shadowShader,ShadowShader)

	Synthesize(Model *,model,AnimationModel)
	Synthesize(ModelAnimClip*,currentClip,CurrentClip)
	Synthesize(Mode	,mode,PlayMode)
	Synthesize(PlayState,playState,PlayState)
	Synthesize(class Transform*,transform,Transform)

	Synthesize(vector<D3DXMATRIX> ,skinTransform,SkinTransform)
	Synthesize(vector<D3DXMATRIX>, boneAnimation,BoneAnimation)

	Synthesize(int,currentKeyframe,CurrentKeyFrame)
	Synthesize(float, frameTime,FrameTime)										//현재 프레임에서 경과된 시간
	Synthesize(float,keyframeFactor,KeyFrameFactor)								 //현재 프레임과 다음 프레임 사이의 보간 값
	Synthesize(bool, useQuaternionKeyframe,UseQuaternionKeyFrame)				//쿼터니언 보간 사용여부

	int nextKeyframe;

	float blendFrameTime;
	float totalBlendingTime;
	float blendTimeFactor;
	int blendStartKeyframe;
	int endClipInex;
	wstring endClipName;
	ModelAnimClip* startBlendClip;
	ModelAnimClip* endBlendClip;
};
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

	void Update();
	void Render();

	void Play();
	void Pause();
	void Stop();

	void ChangeAnimation(wstring animName, function<void()> func = NULL);
	void ChangeAnimation(UINT index);

	void ChangeAnimation(wstring animName, float blendTime);
	void ChangeAnimation(UINT index, float blendTime);

	ModelAnimClip* GetCurrentClip() const { return this->currentClip; }
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
	Shader * shader;
	Shader* shadowShader;

	Model *			model;
	ModelAnimClip*	currentClip;
	Mode			mode;
	PlayState		playState;

	vector<D3DXMATRIX> skinTransform;
	vector<D3DXMATRIX> boneAnimation;

	int currentKeyframe;
	float frameTime; //���� �����ӿ��� ����� �ð�
	float keyframeFactor; //���� �����Ӱ� ���� ������ ������ ���� ��

	int nextKeyframe;
	bool useQuaternionKeyframe; //���ʹϾ� ���� ��뿩��

	float blendFrameTime;
	float totalBlendingTime;
	float blendTimeFactor;
	int blendStartKeyframe;
	int endClipInex;
	wstring endClipName;
	ModelAnimClip* startBlendClip;
	ModelAnimClip* endBlendClip;

};
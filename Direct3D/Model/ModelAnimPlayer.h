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

	ModelAnimPlayer(Model* model);
	~ModelAnimPlayer();

	void Update();
	void Render();

	void Play();
	void Pause();
	void Stop();

	void ChangeAnimation(wstring animName, function<void()> func = NULL);
	void ChangeAnimation(UINT index);
	ModelAnimClip* GetCurrentClip() const { return this->currentClip; }
	Model* GetModel()const { return this->model; }

	vector<D3DXMATRIX> GetBoneAnimations()const { return this->boneAnimation; }
	D3DXMATRIX GetBoneAnimation(UINT index) { return this->boneAnimation[index]; }
	void SetKeyframe(int key);
private:
	void UpdateTime();
	void UpdateBone();

private:
	Shader * shader;
	Shader* shadowShader;

	Model * model;
	ModelAnimClip* currentClip;
	Mode mode;

	int currentKeyframe;
	float frameTime; //���� �����ӿ��� ����� �ð�
	float keyframeFactor; //���� �����Ӱ� ���� ������ ������ ���� ��

	int nextKeyframe;
	bool useQuaternionKeyframe; //���ʹϾ� ���� ��뿩��

	vector<D3DXMATRIX> skinTransform;
	vector<D3DXMATRIX> boneAnimation;
};
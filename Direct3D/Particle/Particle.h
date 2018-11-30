#pragma once
#include "ParticleProp.h"


class ParticleEmitterBase : public GameObject
{
public:
	ParticleEmitterBase(bool bDefault,UINT count);

	virtual~ParticleEmitterBase();

	template <typename T>
	bool SetParticleMaxCount(UINT count = 1024)
	{
		UINT stride = sizeof(T);

		if (stride <= 0) return false;

		maxParticles = count;
		for (int i = 0; i < 2; i++)
		{
			SafeDelete(counterBuffer[i]);
			SafeDelete(particleBuffer[i]);

			particleBuffer[i] = new CResource1D(sizeof(T), maxParticles,nullptr);
			counterBuffer[i] = new CResource1D(sizeof(UINT), 4,nullptr);
		}

	}

public:
	void SetDefault();

	void Init();

	virtual void Update();
	virtual void Render();
	virtual void AlphaRender();
	void SetMode(EmitterMode mode);
	void Emit(int count);

	void UpdateProperty();

	virtual void UIRender();

	virtual void SaveData(Json::Value* parent);
	virtual void LoadData(Json::Value* parent);

protected:
	friend class ParticleTool;
	const int THREAD_NUM_X = 16;

	ComputeShader * particleEmitter;
	ComputeShader * particleUpdater;
	ComputeShader * copyIndirect;
	ComputeShader * resetCounter;

	Shader* particleRenderer;

	CResource1D * particleBuffer[2];
	CResource1D * counterBuffer[2];
	CResourceIndirect* indirectBuffer;

	int currentIndex;
	float loadedGenerateCount;
	float loadedDistanceCount;

	//파티클 생성 및 업데이트
	class CS_EmitData* emitData;
	class CS_UpdateData* updateData;
	class CS_InterpolationData* interpolationData;
	class ParticleAnimation* textureAnimation;

	EmitterMode mode;
	int maxParticles;
	float playTime;
	int countPerSec; //1초동안 생성하는 파티클 총량
	int countPerDis; //거리 변화에 따른 파티클 생성


	bool bLoop;
	bool bFinishEmit;
	bool bAutoRandomSeed;

	float duration;
	float delay;

	bool bUpdate;
	bool bDraw;
	float lastLifeTime;


	vector<BurstInfo> burstDatas;
	vector<BurstInfo> bursts;
	UINT randomSeed;
	//D3DXVECTOR3 oldEmitterPos;


	//렌더링
	RenderMode renderMode;
	ShaderMode shaderMode;
	Texture* particleTexture;

	//Stretched Billboard
	D3DXVECTOR3 oldCameraPos;
	float cameraScale;
	float speedScale;
	float lengthScale;

	//Shape
	

	ShapeType shapeType;
	//ShapeData shapeData;

};


class CS_EmitData : public ShaderBuffer
{
public:
	CS_EmitData() : ShaderBuffer(&Data, sizeof(Data)) {}

private:
	friend class ParticleEmitterBase;
	friend class ParticleTool;

	struct Struct
	{
		float Time;
		D3DXVECTOR3 StartDirection; //초기 설정: ParticleEmitter의 Forward

		float LifeTime;
		D3DXVECTOR3 Gravity;

		D3DXVECTOR3 Force; //가속도
		int ShapeType = 0;

		D3DXVECTOR3 Position;
		int EmitCount;

		D3DXVECTOR2 StartSize;
		D3DXVECTOR2 EndSize;

		float VelocityMax;
		float VelocityMin;
		int RandomRotation;
		int UseRandom;

		D3DXMATRIX EmitterRotation;
		D3DXVECTOR4 EmitterScale;

		float Saturation;
		float Value;
		float Alpha;
		float Color;

		ShapeInfo ShapeData;
	}Data;
};

class CS_UpdateData : public ShaderBuffer
{
public:
	CS_UpdateData() : ShaderBuffer(&Data, sizeof(Data)) {}

private:
	friend class ParticleEmitterBase;
	friend class ParticleTool;

	struct Struct
	{
		float Delta;
		UINT Padding;
		UINT RandomSeed;
		float CameraSpeed;

		D3DXVECTOR3 StretchedScale;
		UINT RenderMode;
	}Data;
};

class CS_InterpolationData : public ShaderBuffer
{
public:
	CS_InterpolationData() : ShaderBuffer(&Data, sizeof(Data)) {}

private:
	friend class ParticleEmitterBase;
	struct Struct
	{
		D3DXVECTOR4 CurveCounts;

		D3DXVECTOR4 ColorCurvePoint[4];
		D3DXVECTOR4 SpeedCurvePoint;
		D3DXVECTOR4 SizeCurvePoint;
	}Data;
};

class ParticleAnimation : public ShaderBuffer
{
public:
	ParticleAnimation() : ShaderBuffer(&Data, sizeof(Data))
	{
		Data.MaxIndex[0] = Data.MaxIndex[1] = 0;
		Data.LoopCount = D3DXVECTOR2(1, 0);
		Data.Fps = D3DXVECTOR3(60.f, 60.f, 80.f);
	}

private:
	friend class ParticleEmitterBase;
	friend class ParticleTool;

	struct Struct
	{
		int MaxIndex[2];
		D3DXVECTOR2 LoopCount;	// bLoop ,count

		D3DXVECTOR3 Fps;
		float Padding;
	}Data;
};
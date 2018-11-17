#pragma once
#include "stdafx.h"



class ParticleEmitterBase : public GameObject
{
public:
	struct ParticleData
	{
		D3DXVECTOR3 Position;
		D3DXCOLOR Color;
		D3DXVECTOR2 Size;
		float Rotation;

		D3DXVECTOR3 Direction;	//방향
		D3DXVECTOR3 Speed;	//가중치

		float LifeTime;	//이놈의 살아있을 시간
		float RemainTime;	//남아있는 수명

		D3DXVECTOR3 Gravity;
		D3DXVECTOR3 Force;

		int NowFrame;
		float FrameTimer;
		float Fps;
		int LoopCount;
		UINT bLoop;

		D3DXVECTOR3 QuadPositions[4];
	};

	struct BurstInfo
	{
		float Time;
		int Count;

		int Cycles;
		float Intervals;

		float Timer;
	};
	enum class EmitterMode { Play , Pause, Stop };
	enum class RenderMode { Billboard  = 0, StretchedBillboard, HorizontalQuad, End };
	enum class ShaderMode { ADDITIVE , MULTIFLY, POW, ALPHABLEND, End };
	enum class ShapeType  { None = 0 , Circle, Sphere, Box, End };

public:
	ParticleEmitterBase(bool bDefault);

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

	void Update(class CameraBase* cam);
	void Render();
	void SetMode(EmitterMode mode);
	void Emit(int count);

	void UpdateProperty();

	void UIRender();

protected:
	friend class ParticleTool;

	const int THREAD_NUM_X = 64;

	ComputeShader * particleEmitter;
	ComputeShader * particleUpdater;
	ComputeShader * copyIndirect;
	ComputeShader * resetCounter;

	Shader* particleRenderer;

	CResource1D * particleBuffer[2];
	CResource1D * counterBuffer[2];
	CResourceIndirect* indirectBuffer;
	int currentIndex;

	int maxParticles;
	//int numParticles;

	float playTime;
	int countPerSec; //1초동안 생성하는 파티클 총량
	int countPerDis; //거리 변화에 따른 파티클 생성 (100m 당 몇개인지.)
	float loadedGenerateCount;
	float loadedDistanceCount;


	EmitterMode mode;
	bool bLoop;
	bool bFinishEmit;
	bool bAutoRandomSeed;

	float duration;
	float delay;

	//파티클 생성 및 업데이트
	class CS_EmitData* emitData;
	class CS_UpdateData* updateData;
	class CS_InterpolationData* interpolationData;
	class ParticleAnimation* textureAnimation;

	vector<BurstInfo> burstDatas;
	vector<BurstInfo> bursts;
	UINT randomSeed;
	D3DXVECTOR3 oldEmitterPos;

	bool bUpdate;
	bool bDraw;
	float lastLifeTime;

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
	struct Struct
	{
		float Time;
		D3DXVECTOR3 StartDirection; //초기 설정: ParticleEmitter의 Forward

		float LifeTime;
		D3DXVECTOR3 Gravity;

		D3DXVECTOR3 Force; //가속도
		int ShapeType;

		D3DXVECTOR3 Position;
		int EmitCount;

		D3DXVECTOR2 StartSize;
		D3DXVECTOR2 EndSize;

		float VelocityMax;
		float VelocityMin;
		int RandomRotation;
		float padding;

		//ParticleEmitterBase::ShapeData ShapeData;
		D3DXMATRIX EmitterRotation;
		D3DXVECTOR4 EmitterScale;

		float Saturation;
		float Value;
		D3DXVECTOR2 padding2;
	}Data;
};

class CS_UpdateData : public ShaderBuffer
{
public:
	CS_UpdateData() : ShaderBuffer(&Data, sizeof(Data)) {}

private:
	friend class ParticleEmitterBase;

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
	friend class ParticleEmitter;
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
		ZeroMemory(Data.MaxIndex, ARRAYSIZE(Data.MaxIndex));
		Data.LoopCount = D3DXVECTOR2(1, 0);
		Data.Fps = D3DXVECTOR3(60.f, 60.f, 80.f);
	}

private:
	friend class ParticleEmitterBase;
	struct Struct
	{
		int MaxIndex[2];
		D3DXVECTOR2 LoopCount;	// bLoop ,count

		D3DXVECTOR3 Fps;
		float Padding;
	}Data;
};
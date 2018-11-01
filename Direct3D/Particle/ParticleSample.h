#pragma once
#include "ParticleBase.h"

struct  ParticleData
{
	bool  IsActive;        // 유효 플래그 
	D3DXVECTOR3  Position;     // 좌표 
	D3DXVECTOR3  Velocity;     // 가속도 
	D3DXCOLOR  Color;          // 색상 
	float  Duration;       // 생존 시간 
	float  Scale;          // 크기 
};

class ParticleSample : public ParticleBase<ParticleData>
{
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Struct))
		{
			Data._EmitPosition = D3DXVECTOR3(0, 0, 0);
			Data._VelocityMax = 1.0f;
			Data._DT = 0.01f;
			Data._Time = 0;
			Data._LifeTime = 1.0f;
			Data._ScaleMin = 0.001f;
			Data._ScaleMax = 10.f;
			Data._Gravity = 0.001f;
			Data._Sai = 0.001f;
			Data._Val = 0.001f;

		}

		struct Struct
		{
			D3DXVECTOR3 _EmitPosition;
			float _VelocityMax;

			float _DT;
			float _Time;
			float _LifeTime;
			float _ScaleMin;

			float _ScaleMax;
			float _Gravity;
			float _Sai;
			float _Val;


		}Data;
	};
public:
	ParticleSample();
	virtual~ParticleSample();
	

	void Update();

	void Render();
	void UIRender();
	void UpdateParticle();
	void EmitParticle(D3DXVECTOR3 pos);
private:
	float velocityMax = 2.f;
	float lifeTime = 3;
	float scaleMin = 1.f;
	float scaleMax = 1.f;
	float gravity = 0.f;

	float sai = 1;   
	float val = 1;   

	Buffer* buffer;
	Shader* shader;

};


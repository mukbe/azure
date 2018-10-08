#pragma once
#include "ParticleBase.h"

struct  ParticleData
{
	bool  IsActive;        // ��ȿ �÷��� 
	D3DXVECTOR3  Position;     // ��ǥ 
	D3DXVECTOR3  Velocity;     // ���ӵ� 
	D3DXCOLOR  Color;          // ���� 
	float  Duration;       // ���� �ð� 
	float  Scale;          // ũ�� 
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
	
	void SetCamera(class FreeCamera* cam) { camera = cam; }

	void Update();

	void Render();

	void UpdateParticle();
	void EmitParticle(D3DXVECTOR3 pos);
private:
	float velocityMax = 1.f;
	float lifeTime = 1;
	float scaleMin = 1;
	float scaleMax = 10;
	float gravity = 0.f;

	float sai = 1;   
	float val = 1;   

	class FreeCamera* camera;
	Buffer* buffer;
	Shader* shader;
	ID3D11Buffer* vertexBuffer;

	WorldBuffer* world;
};


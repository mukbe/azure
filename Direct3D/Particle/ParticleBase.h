#pragma once
#include "stdafx.h"

template <typename T>
class ParticleBase
{
public:
	ParticleBase()
	{
		//��ƼŬ �� ����
		particleNum = (particleMax / THREAD_NUM_X) * THREAD_NUM_X;
		//�ѹ��� �����ϴ� ���� ��
		emitNum = (emitMax / THREAD_NUM_X) * THREAD_NUM_X;

		particleBuffer = new CResource1D(sizeof(T), particleNum, nullptr);
		particlePoolBuffer = new CAppendResource1D(sizeof(UINT), particleNum, nullptr);

		particleCounts = new int[4]{ 0,1,0,0 };
		particleCountBuffer = new CResource1D(sizeof(int), 4, particleCounts);

		initCompute = new ComputeShader(ShaderPath + L"Particle_Compute.hlsl", "Init");
		emitCompute = new ComputeShader(ShaderPath + L"Particle_Compute.hlsl", "Emit");
		updateCompute = new ComputeShader(ShaderPath + L"Particle_Compute.hlsl", "Update");

		initCompute->BindShader();
		particleBuffer->BindResource(0);
		particlePoolBuffer->BindResource(1);
		initCompute->Dispatch(particleNum / THREAD_NUM_X, 1, 1);
		particleBuffer->ReleaseResource(0);
		particlePoolBuffer->ReleaseResource(1);

	}
	virtual~ParticleBase() {}
	
	virtual void Update() {}

	virtual int GetParticleNum() { return particleNum; }
	virtual CResource1D* GetParticleBuffer() { return particleBuffer; }

protected:
	int particleMax = 32;
	int emitMax = 1;
	CAppendResource1D* particlePoolBuffer;
	CResource1D* particleBuffer;
	CResource1D* particleCountBuffer;
	int particleNum ;
	int emitNum ;
	int* particleCounts;
	int particlePoolCount;

	ComputeShader* initCompute;
	ComputeShader* emitCompute;
	ComputeShader* updateCompute;

	const int THREAD_NUM_X = 1;
};


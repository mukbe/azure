#include "stdafx.h"
#include "ParticleSample.h"
#include "./View/FreeCamera.h"


ParticleSample::ParticleSample()
{
	buffer = new Buffer;
	shader = Shaders->CreateShader("Particle", L"Particle.hlsl", Shader::ShaderType::useGS,"Particle");
}


ParticleSample::~ParticleSample()
{
}

void ParticleSample::Update()
{

	if (Mouse::Get()->Press(0))
	{
		EmitParticle(D3DXVECTOR3(0, 0, 0));
	}
	UpdateParticle();

	if (Keyboard::Get()->Down('H'))
	{
		vector<ParticleData> temp;
		temp.assign(particleMax, ParticleData());
		particleBuffer->GetDatas(temp.data());
		int a = 10;
	}
}

void ParticleSample::Render()
{
	States::SetRasterizer(States::RasterizerStates::SOLID_CULL_OFF);

	//DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	shader->Render();
	ID3D11ShaderResourceView* view = particleBuffer->GetSRV();
	DeviceContext->VSGetShaderResources(8, 1, &view);
	DeviceContext->Draw(particleNum,0);

	shader->ReleaseShader();
	States::SetRasterizer(States::RasterizerStates::SOLID_CULL_ON);

}

void ParticleSample::UpdateParticle()
{
	buffer->Data._DT = Time::Delta();
	buffer->Data._LifeTime = lifeTime;
	buffer->Data._Gravity = gravity;

	updateCompute->BindShader();
	buffer->SetCSBuffer(0);
	particleBuffer->BindResource(0);
	particlePoolBuffer->BindResource(1);
	updateCompute->Dispatch(particleNum / THREAD_NUM_X, 1, 1);

	ID3D11UnorderedAccessView* nullUAV[1] = { nullptr };
	DeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
	DeviceContext->CSSetUnorderedAccessViews(1, 1, nullUAV, nullptr);
}

void ParticleSample::EmitParticle(D3DXVECTOR3 pos)
{
	//TODO �Ѱ�ġ �˾Ƴ��� ���Ѱɾ�ߵ�
	//particleCountBuffer.SetData(particleCounts);
	//ComputeBuffer.CopyCount(particlePoolBuffer, particleCountBuffer, 0);
	//particleCountBuffer.GetData(particleCounts);
	//particlePoolNum = particleCounts[0];
	//if (particleCounts[0] < emitNum) return;  
	DeviceContext->CopyStructureCount(particleCountBuffer->GetReadBuffer(), 0, particlePoolBuffer->GetUAV());
	//if (particleCounts[0] < emitNum) return;

	buffer->Data._EmitPosition = pos;
	buffer->Data._VelocityMax = velocityMax;
	buffer->Data._LifeTime = 1.f;
	buffer->Data._ScaleMin = scaleMin;
	buffer->Data._ScaleMax = scaleMax;
	buffer->Data._Sai = sai;
	buffer->Data._Val = val;
	buffer->Data._DT = Time::Delta();
	buffer->Data._LifeTime = lifeTime;
	buffer->Data._Gravity = gravity;
	buffer->Data._Time = Time::Get()->GetWorldTime();

	vector<int> test;
	test.assign(particleNum, int(0));
	particlePoolBuffer->GetDatas(test.data());
	int aa = 10;

	emitCompute->BindShader();
	buffer->SetCSBuffer(0);
	particleBuffer->BindResource(0);
	particlePoolBuffer->BindResource(2);
	emitCompute->Dispatch(1, 1, 1);

	particlePoolBuffer->GetDatas(test.data());
	int aaa = 10;

	vector<ParticleData> temp;
	temp.assign(particleMax, ParticleData());
	particleBuffer->GetDatas(temp.data());
	int a = 10;



	ID3D11UnorderedAccessView* nullUAV[1] = { nullptr };
	DeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
	DeviceContext->CSSetUnorderedAccessViews(2, 1, nullUAV, nullptr);

}
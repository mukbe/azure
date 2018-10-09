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

	if (Mouse::Get()->Down(0))
	{
		EmitParticle(D3DXVECTOR3(0, 0, 0));
	}
	static bool b = true;

	if (Keyboard::Get()->Down('H'))
	{
		b = !b;
	}
	if (Keyboard::Get()->Down('G'))
	{
		vector<ParticleData> temp;
		temp.assign(particleMax, ParticleData());
		particleBuffer->GetDatas(temp.data());
		int a = 10;
	}


	if(b)
		UpdateParticle();


}

void ParticleSample::Render()
{
	shader->Render();

	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ID3D11ShaderResourceView* view = particleBuffer->GetSRV();
	DeviceContext->VSSetShaderResources(8, 1, &view);

	States::SetRasterizer(States::RasterizerStates::SOLID_CULL_OFF);
	{
		DeviceContext->Draw(particleMax, 0);
	}
	States::SetRasterizer(States::RasterizerStates::SOLID_CULL_ON);

	shader->ReleaseShader();
}

void ParticleSample::UIRender()
{
	ImGui::Begin("particle");
	vector<int> data;
	data.assign(4, int(0));
	particleCountBuffer->GetDatas(&data[0]);
	ImGui::Text("count : %d", data[0]);
	ImGui::End();
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
	particleCountBuffer->BindResource(3);
	updateCompute->Dispatch(particleMax / THREAD_NUM_X, 1, 1);

	ID3D11UnorderedAccessView* nullUAV[1] = { nullptr };
	DeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
	DeviceContext->CSSetUnorderedAccessViews(1, 1, nullUAV, nullptr);
	DeviceContext->CSSetUnorderedAccessViews(3, 1, nullUAV, nullptr);
}

void ParticleSample::EmitParticle(D3DXVECTOR3 pos)
{
	//TODO 한계치 알아내서 제한걸어야됨
	//particleCountBuffer.SetData(particleCounts);
	//ComputeBuffer.CopyCount(particlePoolBuffer, particleCountBuffer, 0);
	//particleCountBuffer.GetData(particleCounts);
	//particlePoolNum = particleCounts[0];
	//if (particleCounts[0] < emitNum) return;  

	//vector<int> data;
	//data.assign(4, int(0));
	//particleCountBuffer->GetDatas(&particleCounts);
	//if (particleCounts[0] < emitNum) return;

	buffer->Data._EmitPosition = pos;
	buffer->Data._VelocityMax = velocityMax;
	buffer->Data._ScaleMin = scaleMin;
	buffer->Data._ScaleMax = scaleMax;
	buffer->Data._Sai = sai;
	buffer->Data._Val = val;
	buffer->Data._DT = Time::Delta();
	buffer->Data._LifeTime = lifeTime;
	buffer->Data._Gravity = gravity;
	buffer->Data._Time = Time::Get()->GetWorldTime();

	emitCompute->BindShader();
	buffer->SetCSBuffer(0);
	particleBuffer->BindResource(0);
	particlePoolBuffer->BindResource(2);
	particleCountBuffer->BindResource(3);

	emitCompute->Dispatch(1, 1, 1);

	vector<ParticleData> temp;
	temp.assign(particleMax, ParticleData());
	particleBuffer->GetDatas(temp.data());
	int a = 10;


	ID3D11UnorderedAccessView* nullUAV[1] = { nullptr };
	DeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
	DeviceContext->CSSetUnorderedAccessViews(2, 1, nullUAV, nullptr);

}

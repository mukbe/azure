#include "Particle.h"

#include "./View/FreeCamera.h"

ParticleEmitterBase::ParticleEmitterBase(bool bDefault)
	:currentIndex(0), maxParticles(0), bUpdate(true), bDraw(true)
	, bFinishEmit(false), countPerSec(0), countPerDis(0), loadedGenerateCount(0), loadedDistanceCount(0)
	, playTime(0.f)

{

	particleEmitter = Shaders->CreateComputeShader("ParticleEmitter", L"Particle_Compute_Emit.hlsl", "Emit");
	particleUpdater = Shaders->CreateComputeShader("particleUpdater", L"Particle_Compute_Update.hlsl", "Update");
	copyIndirect = Shaders->CreateComputeShader("CopyIndirect", L"Particle_Compute_CopyIndirect.hlsl", "CopyIndirect");
	resetCounter = Shaders->CreateComputeShader("ResetCounter", L"Particle_Compute_CopyIndirect.hlsl", "ResetCounter");
	particleRenderer = new Shader(ShaderPath + L"Particle_Render.hlsl", Shader::ShaderType::useGS);
	//particleTexture = ResourceManager::Get()->GetTexture(Textures + L"Particles/glow.png");

	//파티클의 키가 달라지면 또 할당하기 때문에 조김해야한다
	//디폴트로 Temp이미지를 둔다
	particleTexture = AssetManager->AddTexture(Assets + L"Temp.png", "Temp");

	for (int i = 0; i < 2; ++i)
	{
		particleBuffer[i] = nullptr;
		counterBuffer[i] = nullptr;
	}

	int* indirect = new int[4]{ 0,1,0,0 };
	indirectBuffer = new CResourceIndirect(indirect);


	emitData = new CS_EmitData;
	updateData = new CS_UpdateData;
	//interpolationData = new CS_InterpolationData;
	textureAnimation = new ParticleAnimation;

	if (bDefault == true)
		SetParticleMaxCount<ParticleData>();

	Init();

	renderMode = RenderMode::Billboard;
}

ParticleEmitterBase::~ParticleEmitterBase()
{
	SafeDelete(emitData);
	SafeDelete(updateData);
	SafeDelete(interpolationData);
	SafeDelete(textureAnimation);

	for (int i = 0; i < 2; ++i)
	{
		SafeDelete(particleBuffer[i]);
		SafeDelete(counterBuffer[i]);
	}
	SafeDelete(indirectBuffer);
}

void ParticleEmitterBase::SetDefault()
{
}

void ParticleEmitterBase::Init()
{
	//GPU버퍼초기화
	counterBuffer[0]->BindResource(0);
	counterBuffer[1]->BindResource(1);
	particleBuffer[currentIndex]->BindResource(2);
	particleBuffer[!currentIndex]->BindResource(3);

	resetCounter->BindShader();
	resetCounter->Dispatch(1, 1, 1);


	ID3D11UnorderedAccessView* uavs[4];
	ZeroMemory(uavs, sizeof(uavs));
	DeviceContext->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);

	//bursts = burstDatas;
	lastLifeTime = 0.0;


	SetMode(EmitterMode::Play);

	//===================test
	delay = 5.0f;
	countPerSec = 5;
	duration = 10.f;
	bLoop = true;
	UpdateProperty();

	//=======================

}

void ParticleEmitterBase::Update(CameraBase* cam)
{
	//발생기의 모드
	if (mode != EmitterMode::Play) return;
	//파티클의 Update
	if (!bUpdate) return;

	//시간에 따른
	float delta = Time::Delta();
	playTime += delta;

	CResource1D* needUpdateBuffer = particleBuffer[currentIndex];
	CResource1D* updatedBuffer = particleBuffer[!currentIndex];
	CResource1D* needUpdateCounter = counterBuffer[currentIndex];
	CResource1D* updatedCounter = counterBuffer[!currentIndex];
	
	float emitCount = (float)countPerSec * delta;

	//딜레이가 있는지
	bool bEmit = playTime >= delay;

	//딜레이가 있고 얼마나 방사하는 시간을 초과했는지 || 루프인지
	bEmit &= (playTime < delay + duration) || bLoop;

	//예약한 시간이 지났는지 && 루프가 아닌지
	if (playTime >= delay + duration + emitData->Data.LifeTime && bLoop == false)
	{
		//끝났을경우 할 명령

	}

	//생성 카운트 소수점 보정.
	loadedGenerateCount += emitCount - (int)emitCount;
	emitCount = (float)((int)emitCount);

	if (loadedGenerateCount >= 1.0f)
	{
		++emitCount;
		loadedGenerateCount = 0.0f;
	}
	if (loadedDistanceCount >= 1.0f)
	{
		//++distanceCount;
		//loadedDistanceCount = 0.0f;
	}

	//방사
	if (bEmit && bFinishEmit == false)
	{
		if (emitCount > 0)
			Emit((int)emitCount);
	}


	if (true)
	{
		if (bAutoRandomSeed) randomSeed = Math::Random(0, 5000000);

		updateData->Data.Delta = delta;
		updateData->Data.RandomSeed = randomSeed;
		updateData->Data.CameraSpeed = 1.0f;// D3DXVec3Length(&(camPosition - oldCameraPos));

		updateData->Data.RenderMode = (UINT)renderMode;
		updateData->Data.StretchedScale = D3DXVECTOR3(cameraScale, speedScale, lengthScale);

		updateData->SetCSBuffer(2);
		//interpolationData->SetCSBuffer(2);
		//textureAnimation->SetCSBuffer(3);

		emitData->SetCSBuffer(1);
		cam->Render();
		//=========================================================

		needUpdateBuffer->BindResource(0);
		updatedBuffer->BindResource(1);
		needUpdateCounter->BindResource(2);
		updatedCounter->BindResource(3);

		particleUpdater->BindShader();
		particleUpdater->Dispatch(1, 1, 1);


		//Indirect Buffer 갱신
		updatedCounter->BindResource(0);
		indirectBuffer->BindResource(4);

		copyIndirect->BindShader();
		copyIndirect->Dispatch(1, 1, 1);

		ID3D11UnorderedAccessView* uavs[5];
		ZeroMemory(uavs, sizeof(uavs));
		DeviceContext->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, 0);

		//=========================================================

		// - Current Buffer 스왑
		currentIndex = !currentIndex;
	}

}

void ParticleEmitterBase::Render()
{
	if (!bDraw) return;

	//파티클 렌더링
	ID3D11ShaderResourceView* particleData = particleBuffer[currentIndex]->GetSRV();
	ID3D11ShaderResourceView* nullView = nullptr;

	textureAnimation->SetPSBuffer(3);

	particleRenderer->Render();
	particleTexture->SetShaderResource(0);

	DeviceContext->VSSetShaderResources(0, 1, &particleData);
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	States::SetRasterizer(States::SOLID_CULL_OFF);
	//switch (shaderMode)
	//{
	//case ParticleEmitter::ShaderMode::ADDITIVE:
	//	States::SetBlend(States::BLENDING_ADDITIVE);
	//	break;
	//case ParticleEmitter::ShaderMode::MULTIFLY:
	//	States::SetBlend(States::BLENDING_MULTIFLY);
	//	break;
	//case ParticleEmitter::ShaderMode::POW:
	//	States::SetBlend(States::BLENDING_POW);
	//	break;
	//case ParticleEmitter::ShaderMode::ALPHABLEND:
	//	States::SetBlend(States::BLENDING_ON);
	//	break;
	//}
	{
		DeviceContext->DrawInstancedIndirect(indirectBuffer->GetIndirectBuffer(), 0);
	}
	//States::SetDepthStencil(States::DEPTH_ON);
	//States::SetBlend(States::BLENDING_OFF);
	States::SetRasterizer(States::SOLID_CULL_ON);

	DeviceContext->VSSetShaderResources(0, 1, &nullView);
	DeviceContext->PSSetShaderResources(0, 1, &nullView);
	particleRenderer->ReleaseShader();

}

void ParticleEmitterBase::SetMode(EmitterMode mode)
{
	this->mode = mode;
	switch (mode)
	{
		case ParticleEmitterBase::EmitterMode::Play:	bUpdate = true;
		break;
		case ParticleEmitterBase::EmitterMode::Pause:	bUpdate = false;
		break;
		case ParticleEmitterBase::EmitterMode::Stop:
		break;
	}
}

void ParticleEmitterBase::Emit(int count)
{
	Transform* trans = this->GetTransform();
	
	//emitData->Data.GenerateCount = (int)count;
	emitData->Data.Time = Time::Get()->GetWorldTime();
	emitData->Data.Position = trans->GetWorldPosition();
	emitData->Data.StartDirection = trans->GetForward();
	emitData->Data.ShapeType = 0;
	emitData->Data.LifeTime = 3.f;
	//emitData->Data.ShapeData = shapeData;
	//emitData->Data.ShapeData.CircleAngle = Math::ToRadian(shapeData.CircleAngle);

	//D3DXQUATERNION qRot = trans->get ->GetLocalQuaternion();
	//D3DXMatrixRotationQuaternion(&generateData->Data.EmitterRotation, &qRot);
	//D3DXMatrixTranspose(&generateData->Data.EmitterRotation, &generateData->Data.EmitterRotation);


	D3DXVECTOR3 localScale = trans->GetScale();
	emitData->Data.EmitterScale = { localScale.x, localScale.y, localScale.z, 1.0f };

	//emitData->UpdateData();
	emitData->SetCSBuffer(0);
	textureAnimation->SetCSBuffer(3);



	particleBuffer[currentIndex]->BindResource(0);
	counterBuffer[currentIndex]->BindResource(1);
	particleEmitter->BindShader();

	int dispatchCount = (int)(count / THREAD_NUM_X) + 1;
	particleEmitter->Dispatch(dispatchCount, 1, 1);


	ID3D11UnorderedAccessView* uavs[2] = { nullptr, nullptr };
	ZeroMemory(uavs, sizeof(uavs));
	DeviceContext->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, 0);



}

void ParticleEmitterBase::UpdateProperty()
{
	emitData->Data.Gravity = D3DXVECTOR3(0,0,0);
	emitData->Data.Force = D3DXVECTOR3(0,0,0);
	emitData->Data.Position = D3DXVECTOR3(0,0,0);
	emitData->Data.StartSize = D3DXVECTOR2(0.01f, 0.01f);
	emitData->Data.EndSize = D3DXVECTOR2(0.01f, 0.01f);

	emitData->Data.Saturation = 0.1f;
	emitData->Data.Value = 0.1f;

	emitData->Data.VelocityMax = 1.5f;
	emitData->Data.VelocityMin = 0.1f;
}

void ParticleEmitterBase::UIRender()
{
	ImGui::SliderFloat("Saturation", &emitData->Data.Saturation, 0.0001f, 1.0f);
	ImGui::SliderFloat("Value", &emitData->Data.Value, 0.0001f, 1.0f);
	ImGui::SliderFloat2("StartSize", &emitData->Data.StartSize.x, 0.0001f, 1.0f);
	ImGui::SliderFloat2("EndSize", &emitData->Data.EndSize.x, 0.0001f, 1.0f);
	ImGui::SliderFloat("VelocityMax", &emitData->Data.VelocityMax, 0.0f, 10.0f);
	ImGui::SliderFloat("VelocityMin", &emitData->Data.VelocityMin, 0.0f, 10.0f);

	ImGui::SliderInt2("MaxIndex", (&textureAnimation->Data.MaxIndex[0]), 0, 10);
	ImGui::SliderFloat("FPS", &textureAnimation->Data.Fps.x, 0.f, 80.f);
}


#include "Particle.h"
#include "./Utilities/ImGuiHelper.h"


ParticleEmitterBase::ParticleEmitterBase(bool bDefault, UINT count)
	:currentIndex(0), maxParticles(0), bUpdate(true), bDraw(true)
	, bFinishEmit(false), countPerSec(0), countPerDis(0), loadedGenerateCount(0), loadedDistanceCount(0)
	, playTime(0.f)

{
	particleEmitter = Shaders->CreateComputeShader("ParticleEmitter", L"Particle_Compute_Emit.hlsl", "Emit");
	particleUpdater = Shaders->CreateComputeShader("particleUpdater", L"Particle_Compute_Update.hlsl", "Update");
	copyIndirect = Shaders->CreateComputeShader("CopyIndirect", L"Particle_Compute_CopyIndirect.hlsl", "CopyIndirect");
	resetCounter = Shaders->CreateComputeShader("ResetCounter", L"Particle_Compute_CopyIndirect.hlsl", "ResetCounter");
	particleRenderer = new Shader(ShaderPath + L"Particle_Render.hlsl", Shader::ShaderType::useGS);

	//파티클의 키가 달라지면 또 할당하기 때문에 조김해야한다
	//디폴트로 Temp이미지를 둔다
	particleTexture = AssetManager->AddTexture(Assets + L"Temp3.png", "Temp3");

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
		SetParticleMaxCount<ParticleData>(count);

	Init();

	renderMode = RenderMode::Billboard;

}

ParticleEmitterBase::~ParticleEmitterBase()
{
	SafeDelete(emitData);
	SafeDelete(updateData);
	//SafeDelete(interpolationData);
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


	mode = EmitterMode::Stop;

	//===================test
	delay = 0.0f;
	countPerSec = 20;
	countPerDis = 0;
	duration = 10.f;
	bLoop = true;
	UpdateProperty();

	//=======================

}

void ParticleEmitterBase::Update()
{
	//발생기의 모드
	if (mode != EmitterMode::Play) return;
	//파티클의 Update
	if (!bUpdate) return;

	//시간에 따른
	float delta = Time::Delta();
	playTime += delta;

	CResource1D* consumeBuffer = particleBuffer[currentIndex];
	CResource1D* appendBuffer = particleBuffer[!currentIndex];
	CResource1D* consumeCounter = counterBuffer[currentIndex];
	CResource1D* appendCounter = counterBuffer[!currentIndex];
	
	D3DXVECTOR3 position = this->GetTransform()->GetWorldPosition();
	float distance = D3DXVec3Length(&(emitData->Data.Position - position));

	float emitCount = (float)countPerSec * delta;
	float distanceCount = (float)countPerDis * distance;

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

	loadedDistanceCount += distanceCount - (int)distanceCount;
	distanceCount = (float)((int)distanceCount);

	if (loadedGenerateCount >= 1.0f)
	{
		++emitCount;
		loadedGenerateCount = 0.0f;
	}
	if (loadedDistanceCount >= 1.0f)
	{
		++distanceCount;
		loadedDistanceCount = 0.0f;
	}

	//방사
	if (bEmit && bFinishEmit == false)
	{
		if (emitCount > 0)
			Emit((int)emitCount);
		if (distanceCount > 0)
			Emit((int)distanceCount);

		//자동생성 추가
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
		textureAnimation->SetCSBuffer(3);
		//interpolationData->SetCSBuffer(2);


		emitData->SetCSBuffer(1);
		Cameras->BindGPU("FreeCamera");
		//=========================================================

		consumeBuffer->BindResource(0);
		appendBuffer->BindResource(1);
		consumeCounter->BindResource(2);
		appendCounter->BindResource(3);

		particleUpdater->BindShader();
		particleUpdater->Dispatch(1, 1, 1);


		//Indirect Buffer 갱신
		appendCounter->BindResource(0);
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
	

}

void ParticleEmitterBase::AlphaRender()
{
	if (!bDraw) return;

	//파티클 렌더링
	ID3D11ShaderResourceView* particleData = particleBuffer[currentIndex]->GetSRV();
	ID3D11ShaderResourceView* nullView = nullptr;

	textureAnimation->SetPSBuffer(3);

	particleRenderer->Render();
	particleTexture->SetShaderResource(0);


	States::SetRasterizer(States::SOLID_CULL_OFF);
	//switch (shaderMode)
	//{
	//case ShaderMode::ADDITIVE:
	//	States::SetBlend(States::BLENDING_ADDITIVE);
	//	break;
	//case ShaderMode::MULTIFLY:
	//	States::SetBlend(States::BLENDING_MULTIFLY);
	//	break;
	//case ShaderMode::POW:
	//	States::SetBlend(States::BLENDING_POW);
	//	break;
	//case ShaderMode::ALPHABLEND:
	//	States::SetBlend(States::BLENDING_ON);
	//	break;
	//}
	States::SetBlend(States::BLENDING_ON);

	DeviceContext->VSSetShaderResources(0, 1, &particleData);
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	{
		DeviceContext->DrawInstancedIndirect(indirectBuffer->GetIndirectBuffer(), 0);
	}
	States::SetBlend(States::BLENDING_OFF);
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
		case EmitterMode::Play:		bUpdate = true;
		break;
		case EmitterMode::Pause:	bUpdate = false;
		break;
		case EmitterMode::Stop:
		{
			Init();
		}
		break;
	}
}

void ParticleEmitterBase::Emit(int count)
{
	
	//emitData->Data.GenerateCount = (int)count;
	emitData->Data.Time = Time::Get()->GetWorldTime();

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
	Transform* trans = this->GetTransform();

	emitData->Data.Position = trans->GetWorldPosition();
	emitData->Data.StartDirection = trans->GetForward();
	emitData->Data.ShapeType = 0;
	emitData->Data.LifeTime = 3.f;

	D3DXVECTOR3 localScale = trans->GetScale();
	emitData->Data.EmitterScale = { localScale.x, localScale.y, localScale.z, 1.0f };

	//emitData->Data.ShapeData = shapeData;
	//emitData->Data.ShapeData.CircleAngle = Math::ToRadian(shapeData.CircleAngle);

	D3DXMATRIX mat = trans->GetRotateMatrix();
	//D3DXMatrixRotationQuaternion(&generateData->Data.EmitterRotation, &qRot);
	D3DXMatrixTranspose(&emitData->Data.EmitterRotation, &mat);

	emitData->Data.Gravity = D3DXVECTOR3(0,1.0f,0);
	emitData->Data.Force = D3DXVECTOR3(0,0,0);
	emitData->Data.Position = D3DXVECTOR3(0,0,0);
	emitData->Data.StartSize = D3DXVECTOR2(0.01f, 0.01f);
	emitData->Data.EndSize = D3DXVECTOR2(0.01f, 0.01f);
	emitData->Data.UseRandom = 1;

	emitData->Data.Saturation = 0.1f;
	emitData->Data.Value = 0.1f;
	emitData->Data.Alpha = 1.f;
	emitData->Data.Color = 0.f;


	emitData->Data.VelocityMax = 1.5f;
	emitData->Data.VelocityMin = 0.1f;
}

void ParticleEmitterBase::UIRender()
{
	static bool emitterMode = false;
	if (ImGui::Checkbox("EmitterMode", &emitterMode))
	{
		if (emitterMode == true) mode = EmitterMode::Play;
		else mode = EmitterMode::Pause;
	}
	ImGui::Checkbox("Loop", &bLoop);
	ImGui::Checkbox("Update", &bUpdate);
	ImGui::Checkbox("Render", &bDraw);

	ImGui::InputFloat("delayTime", &delay, 0.5f, 1.0f);
	ImGui::InputFloat("duration", &duration, 0.5f, 1.0f);
	ImGui::InputInt("CountPerSec", &countPerSec, 0.5f, 1.0f);
	ImGui::InputInt("CountPerDis", &countPerDis, 0.5f, 1.0f);

	static int maxCount = maxParticles;
	ImGuiInputTextFlags flag = ImGuiInputTextFlags_EnterReturnsTrue;
	if (ImGui::InputInt("MaxParticles", &maxCount,1,100, flag))
	{
		SetParticleMaxCount<ParticleData>(maxCount);
		SetMode(EmitterMode::Stop);
	}


	static bool random = (bool)emitData->Data.UseRandom;
	if (ImGui::Checkbox("Random", &random))
	{
		emitData->Data.UseRandom = !emitData->Data.UseRandom;
	}
	ImGui::SliderFloat("Saturation", &emitData->Data.Saturation, 0.0001f, 1.0f, "%.3f");
	ImGui::SliderFloat("Value", &emitData->Data.Value, 0.0001f, 1.0f, "%.3f");
	ImGui::SliderFloat("Alpha", &emitData->Data.Alpha, 0.0001f, 1.0f, "%.3f");
	ImGui::SliderFloat("Color", &emitData->Data.Color, 0.0f, 1.0f, "%.2f");

	ImGui::InputFloat("LifeTime", &emitData->Data.LifeTime, 1.f, 5.f, -1, flag);
	ImGui::InputFloat3("EmitterScale", &emitData->Data.EmitterScale.x, -1, flag);
	ImGui::InputFloat2("StartSize", &emitData->Data.StartSize.x, -1, flag);
	ImGui::InputFloat2("EndSize", &emitData->Data.EndSize.x, -1, flag);
	ImGui::InputFloat("VelocityMax", &emitData->Data.VelocityMax, 1.0f, 10.0f,-1 , flag);
	ImGui::InputFloat("VelocityMin", &emitData->Data.VelocityMin, 1.0f, 10.0f,-1 , flag);
	ImGui::InputFloat3("Gravity", &emitData->Data.Gravity.x, -1, flag);
	ImGui::InputFloat3("Force", &emitData->Data.Force.x, -1, flag);
	ImGui::InputInt("Shape", &emitData->Data.ShapeType, 1, 2, flag);
	ImGui::InputFloat2("RadiusSize", &emitData->Data.ShapeData.RadiusRange.x, -1, flag);
	static bool inverse = (bool)emitData->Data.ShapeData.Inverse;
	if (ImGui::Checkbox("Inverse", &inverse))
		emitData->Data.ShapeData.Inverse = inverse ? 1 : 0;
	ImGui::SliderAngle("Range", &emitData->Data.ShapeData.CircleAngle, 0.f);

	if (ImGui::InputInt2("MaxIndex", (&textureAnimation->Data.MaxIndex[0]), flag))
	{
		textureAnimation->Data.MaxIndex[0] = Math::Clamp(textureAnimation->Data.MaxIndex[0], 0, 15);
		textureAnimation->Data.MaxIndex[1] = Math::Clamp(textureAnimation->Data.MaxIndex[1], 0, 15);
	}

	if (ImGui::InputFloat("TextureLoop", &textureAnimation->Data.LoopCount.x, 1, 2, -1, flag))
		textureAnimation->Data.LoopCount.x = Math::Clamp(textureAnimation->Data.LoopCount.x, 0, 1);

	ImGui::InputFloat("TextureLoopCount", &textureAnimation->Data.LoopCount.y, 1.0f, 2.0f, flag);
	if (ImGui::InputFloat("FPS", &textureAnimation->Data.Fps.x, 1.f, 10.f, -1, flag))
		textureAnimation->Data.Fps.x = Math::Clamp(textureAnimation->Data.Fps.x, 0.f, 360.f);


	ImGuiHelper::RenderImageButton(&particleTexture, ImVec2(100, 100));

}

void ParticleEmitterBase::SaveData(Json::Value * parent)
{
	Json::Value prop;


}

void ParticleEmitterBase::LoadData(Json::Value * parent)
{
}


#include "stdafx.h"
#include "Scattering.h"

#include "Environment/Sun.h"
#include "./View/FreeCamera.h"
#include "./Bounding/BoundingFrustum.h"
#include "./View/OrthoWindow.h"
#include "./Utilities/Transform.h"


Scattering::Scattering(FreeCamera* camera , string level)
{
	jsonRoot = new Json::Value();
	//TODO level에서 받아오도록
	JsonHelper::ReadData(L"ScatteringEditor.json", jsonRoot);
	Json::Value prop = (*jsonRoot)["Property"];
	if (prop.isNull() == false)
	{
		JsonHelper::GetValue(prop, "SampleCount", SampleCount);
		JsonHelper::GetValue(prop, "IncomingLight", IncomingLight);
		JsonHelper::GetValue(prop, "RayleighScatterCoef", RayleighScatterCoef);
		JsonHelper::GetValue(prop, "RayleighExtinctionCoef", RayleighExtinctionCoef);
		JsonHelper::GetValue(prop, "MieScatterCoef", MieScatterCoef);
		JsonHelper::GetValue(prop, "MieExtinctionCoef", MieExtinctionCoef);
		JsonHelper::GetValue(prop, "MieG", MieG);
		JsonHelper::GetValue(prop, "LightColorIntensity", LightColorIntensity);
		JsonHelper::GetValue(prop, "AmbientColorIntensity", AmbientColorIntensity);
		JsonHelper::GetValue(prop, "SunIntensity", SunIntensity);

		int renderMode;
		JsonHelper::GetValue(prop, "RenderMode", renderMode);
		RenderingMode = (RenderMode)renderMode;

	}

	//쉐이더 초기화
	shader = Shaders->CreateShader("Scattering", L"AtmosphericScatteringSkyBox.hlsl");
	skyBoxShader = Shaders->CreateShader("ScatteringUseSkyBox", L"AtmosphericScatteringSkyBox.hlsl", Shader::ShaderType::Default, "UseSkyBox");

	world = Buffers->FindShaderBuffer<WorldBuffer>();

	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);
	world->SetMatrix(mat);

	sun = new Environment::Sun();
	buffer = new Buffer;

	_camera = camera;

	particleDensityLUTComputeShader = new ComputeShader(ShaderPath + L"AtmosphericScattering_Compute.hlsl", "particleDensityLUT");
	precomputeSkyboxLUT = new ComputeShader(ShaderPath + L"AtmosphericScattering_Compute.hlsl", "SkyboxLUT");

	UpdateMaterialParameters();

	PrecomputeParticleDensity();
	CalculateLightLUTs();

	CreateBuffer();
}


Scattering::~Scattering()
{
	//TODO sun의 위치 
	Json::Value prop;
	JsonHelper::SetValue(prop, "SampleCount", SampleCount);
	JsonHelper::SetValue(prop, "IncomingLight", IncomingLight);
	JsonHelper::SetValue(prop, "RayleighScatterCoef", RayleighScatterCoef);
	JsonHelper::SetValue(prop, "RayleighExtinctionCoef", RayleighExtinctionCoef);
	JsonHelper::SetValue(prop, "MieScatterCoef", MieScatterCoef);
	JsonHelper::SetValue(prop, "MieExtinctionCoef", MieExtinctionCoef);
	JsonHelper::SetValue(prop, "MieG", MieG);
	JsonHelper::SetValue(prop, "LightColorIntensity", LightColorIntensity);
	JsonHelper::SetValue(prop, "AmbientColorIntensity", AmbientColorIntensity);
	JsonHelper::SetValue(prop, "SunIntensity", SunIntensity);

	int renderMode = RenderingMode;
	JsonHelper::SetValue(prop, "RenderMode", renderMode);

	(*jsonRoot)["Property"] = prop;
	JsonHelper::WriteData(L"ScatteringEditor.json", jsonRoot);
	SafeDelete(jsonRoot);

	SafeRelease(vertexBuffer);
	SafeRelease(indexBuffer);

	SafeDelete(_particleDensityLUT);
	SafeDelete(precomputeSkyboxLUT);
	SafeDelete(particleDensityLUTComputeShader);
	SafeDelete(buffer);
	SafeDelete(sun);
}

void Scattering::Updata()
{
	sun->Update();
	sun->UpdateView();

	buffer->Data._SunColor = ComputeLightColor();
	UpdateDirectionalLightColor(buffer->Data._SunColor);
	UpdateAmbientLightColor(ComputeAmbientColor());

}

void Scattering::Render()
{
	//드로우
	UpdateMaterialParameters();

	//TODO 카메라의 world를 쓰는것이다 
	Transform tranform;
	tranform.SetWorldPosition(_camera->GetTransform()->GetWorldPosition());
	D3DXMATRIX mat = tranform.GetFinalMatrix();
	world->SetMatrix(mat);

	_particleDensityLUT->BindPSShaderResourceView(0);
	_skyboxLUT->BindPSShaderResourceView(1);

	if(RenderingMode == RenderMode::Reference)
		shader->Render();
	else
		skyBoxShader->Render();

	world->SetPSBuffer(1);
	world->SetVSBuffer(1);
	buffer->SetPSBuffer(2);
	sun->Render();

	UINT stride = sizeof(VertexTextureNormalTangent);
	UINT offset = 0;

	DeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	DeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	States::SetRasterizer(States::RasterizerStates::SOLID_CULL_OFF);
	pRenderer->ChangeZBuffer(false);
	DeviceContext->DrawIndexed(36, 0, 0);
	pRenderer->ChangeZBuffer(true);
	States::SetRasterizer(States::RasterizerStates::SOLID_CULL_ON);
	
}

void Scattering::UIRender()
{
	ImGui::Begin("Scattering");

	D3DXMATRIX mat = _camera->GetTransform()->GetFinalMatrix();
	ImGui::Text("CameraX : %.2f , CameraY : %.2f, CameraZ : %.2f", mat._41,mat._42,mat._43);

	if (ImGui::InputInt("SampleCount", &SampleCount, 1))
		SampleCount = Math::Clamp(SampleCount, 1, 64);
	
	if (ImGui::ColorEdit4("IncomingLight", IncomingLight))
		IncomingLight *= 4.0f; 

	ImGui::SliderFloat("RayleighScatterCoef", &RayleighScatterCoef, 0.f, 10.f);
	ImGui::SliderFloat("RayleighExtinctionCoef", &RayleighExtinctionCoef, 0.f, 10.f);
	ImGui::SliderFloat("MieScatterCoef", &MieScatterCoef, 0.f, 10.f);
	ImGui::SliderFloat("MieExtinctionCoef", &MieExtinctionCoef, 0.f, 10.f);
	ImGui::SliderFloat("MieG", &MieG, 0.f, 0.996f);

	ImGui::SliderFloat("LightColorIntensity", &LightColorIntensity, 0.5f, 3.f);
	ImGui::SliderFloat("AmbientColorIntensity", &AmbientColorIntensity, 0.5f, 3.f);
	ImGui::SliderFloat("SunIntensity", &SunIntensity, 0.f, 2.f);

	const char* ThemesList[] = { "Reference" , "Optimized"  };
	ImGui::Combo("RenderMode", (int*)&RenderingMode, ThemesList, 2);
	ImGui::End();
}

void Scattering::UpdateMaterialParameters()
{
	buffer->Data._AtmosphereHeight = AtmosphereHeight;
	buffer->Data._PlanetRadius = PlanetRadius;
	buffer->Data._DensityScaleHeight = DensityScale;

	buffer->Data._ScatteringR = RayleighSct * RayleighScatterCoef;
	buffer->Data._ScatteringM = MieSct * MieScatterCoef;
	buffer->Data._ExtinctionR = RayleighSct * RayleighExtinctionCoef;
	buffer->Data._ExtinctionM = MieSct * MieExtinctionCoef;

	buffer->Data._IncomingLight = IncomingLight;
	buffer->Data._MieG = MieG;
	buffer->Data._DistanceScale = DistanceScale;
	//buffer->Data._SunColor = _sunColor;
	buffer->Data._SunIntensity = SunIntensity;

	sun->SetColor(buffer->Data._SunColor);

	D3DXVECTOR3 forward = sun->GetForward();
	buffer->Data._LightDir = D3DXVECTOR4(forward.x, forward.y, forward.z, 1.0f / 100.f);
}

void Scattering::PrecomputeParticleDensity()
{
	_particleDensityLUT = new CResource2D(720, 720);
	
	particleDensityLUTComputeShader->BindShader();
	buffer->SetCSBuffer(2);

	_particleDensityLUT->BindResource(0);
	particleDensityLUTComputeShader->Dispatch(45,45, 1);

	_particleDensityLUT->ReleaseResource(0);

}

void Scattering::CalculateLightLUTs()
{
	Texture* temp = new Texture(Contents + L"Atmospheric/AmbientLUT.png");
	_ambientLightLUT.assign(LightLUTSize, D3DXCOLOR());
	temp->GetPixel(_ambientLightLUT);
	SafeDelete(temp);

	temp = new Texture(Contents + L"Atmospheric/DirectionalLUT.png");
	_directionalLightLUT.assign(LightLUTSize, D3DXCOLOR());
	temp->GetPixel(_directionalLightLUT);
	SafeDelete(temp);


	PrecomputeSkyboxLUT();
}


void Scattering::PrecomputeSkyboxLUT()
{
	_skyboxLUT = new CResource3D((int)_skyboxLUTSize.x, (int)_skyboxLUTSize.y ,(int)_skyboxLUTSize.z,DXGI_FORMAT_R16G16B16A16_FLOAT);

	//SkyboxLUT compute dispatch;
	precomputeSkyboxLUT->BindShader();
	buffer->SetCSBuffer(2);

	_skyboxLUT->BindResource(1);
	_particleDensityLUT->BindCSShaderResourceView(0); //컴퓨팅으로 대신 그려놨음

	precomputeSkyboxLUT->Dispatch((int)_skyboxLUTSize.x, (int)_skyboxLUTSize.y, (int)_skyboxLUTSize.z);

	_skyboxLUT->ReleaseResource(1);
	_particleDensityLUT->ReleaseCSshaderResorceView(0);
}


D3DXCOLOR Scattering::ComputeLightColor()
{
	//TODO SunForward반대로 되있으므로 트랜스폼 점검
	float cosAngle = -D3DXVec3Dot(&D3DXVECTOR3(0, 1, 0), &sun->GetForward());
	float u = (cosAngle + 0.1f) / 1.1f;// * 0.5f + 0.5f;

	u = u * LightLUTSize;
	int index0 = (int)(u);
	float weight1 = u - index0;
	int index1 = index0 + 1;
	float weight0 = 1 - weight1;

	index0 = Math::Clamp(index0, 0, LightLUTSize - 1);
	index1 = Math::Clamp(index1, 0, LightLUTSize - 1);

	D3DXCOLOR c = _directionalLightLUT[index0] * weight0 + _directionalLightLUT[index1] * weight1;
	c /= 2.2f;
	
	return c;
}

void Scattering::UpdateDirectionalLightColor(D3DXCOLOR sunColor)
{
	D3DXVECTOR3 color = D3DXVECTOR3(sunColor.r, sunColor.g, sunColor.b);
	float length = D3DXVec3Length(&color);
	color /= length;

	buffer->Data._SunColor = D3DXCOLOR(Math::Max(color.x, 0.01f), Math::Max(color.y, 0.01f), Math::Max(color.z, 0.01f), 1);
	buffer->Data._SunIntensity = Math::Max(length, 0.01f) * LightColorIntensity; // make sure unity doesn't disable this light
}

void Scattering::UpdateAmbientLightColor(D3DXCOLOR ambient)
{
	//너무 어두워서 조금 보정해주는것
	buffer->Data._AddAmbient = ambient;
}

D3DXCOLOR Scattering::ComputeAmbientColor()
{
	float cosAngle = D3DXVec3Dot(&D3DXVECTOR3(0, -1, 0), &sun->GetForward());
	float u = (cosAngle + 0.1f) / 1.1f;// * 0.5f + 0.5f;

	u = u * LightLUTSize;
	int index0 = (int)(u);
	float weight1 = u - index0;
	int index1 = index0 + 1;
	float weight0 = 1 - weight1;

	index0 = Math::Clamp(index0, 0, LightLUTSize - 1);
	index1 = Math::Clamp(index1, 0, LightLUTSize - 1);

	D3DXCOLOR c = _ambientLightLUT[index0] * weight0 + _ambientLightLUT[index1] * weight1;
	//c /= 2.2f;

	return c;
}

void Scattering::CreateBuffer()
{
	GeometryGenerator geo;

	geo.CreateBox(0.5f, 0.5f, 0.5f, meshData);


	//VertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTextureNormalTangent) * meshData.Vertices.size();
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = meshData.Vertices.data();

		HRESULT hr;
		hr = Device->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//IndexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * meshData.Indices.size();
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data = { 0 };
		data.pSysMem = meshData.Indices.data();

		HRESULT hr;
		hr = Device->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}

}

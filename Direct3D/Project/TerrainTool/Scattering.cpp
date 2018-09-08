#include "stdafx.h"
#include "Scattering.h"

#include "Environment/Sun.h"
#include "./View/FreeCamera.h"
#include "./Bounding/BoundingFrustum.h"
#include "./View/OrthoWindow.h"
#include "./Utilities/Transform.h"


Scattering::Scattering(FreeCamera* camera)
{
	//쉐이더 초기화
	shader = Shaders->CreateShader("Scattering", L"AtmosphericScatteringSkyBox.hlsl");
	world = Buffers->FindShaderBuffer<WorldBuffer>();
	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);

	world->SetMatrix(mat);

	sun = new Environment::Sun();

	//메터리얼 초기화
	buffer = new Buffer;

	_camera = camera;

	particleDensityLUTComputeShader = new ComputeShader(ShaderPath + L"AtmosphericScattering.hlsl", "particleDensityLUT");
	precomputeSkyboxLUT = new ComputeShader(ShaderPath + L"AtmosphericScattering_Compute.hlsl", "SkyboxLUT");
	ScatteringComputeShader = new ComputeShader(ShaderPath + L"AtmosphericScattering_Compute.hlsl", "InscatteringLUT");
	UpdateMaterialParameters();

	PrecomputeParticleDensity();
	CalculateLightLUTs();

	InitializeInscatteringLUT();

	CreateBuffer();
}


Scattering::~Scattering()
{
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
	_FrustumCorners[0] = _camera->GetFrustum()->transformVertex[6];
	_FrustumCorners[1] = _camera->GetFrustum()->transformVertex[4];
	_FrustumCorners[2] = _camera->GetFrustum()->transformVertex[5];
	_FrustumCorners[3] = _camera->GetFrustum()->transformVertex[7];

	//컴퓨팅
	UpdateMaterialParameters();
	UpdateInscatteringLUT();
	
	Transform tranform;
	tranform.SetWorldPosition(_camera->GetTransform()->GetWorldPosition());

	D3DXMATRIX mat = tranform.GetFinalMatrix();
	world->SetMatrix(mat);

	//드로우
	_particleDensityLUT->BindPSShaderResourceView(0);
	shader->Render();
	world->SetPSBuffer(1);
	world->SetVSBuffer(1);
	buffer->SetPSBuffer(2);


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
	
	ImGui::ColorEdit4("IncomingLight", IncomingLight);
	ImGui::SliderFloat("RayleighScatterCoef", &RayleighScatterCoef, 0.f, 10.f);
	ImGui::SliderFloat("RayleighExtinctionCoef", &RayleighExtinctionCoef, 0.f, 10.f);
	ImGui::SliderFloat("MieScatterCoef", &MieScatterCoef, 0.f, 10.f);
	ImGui::SliderFloat("MieExtinctionCoef", &MieExtinctionCoef, 0.f, 10.f);
	ImGui::SliderFloat("MieG", &MieG, 0.f, 0.996f);

	ImGui::SliderFloat("LightColorIntensity", &LightColorIntensity, 0.5f, 3.f);
	ImGui::SliderFloat("AmbientColorIntensity", &AmbientColorIntensity, 0.5f, 3.f);
	ImGui::SliderFloat("SunIntensity", &SunIntensity, 0.f, 100.f);



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
	buffer->Data._SunColor = _sunColor = D3DXCOLOR(0.5f,0.5f,0.5f, 1);

	D3DXVECTOR3 forward =   sun->GetForward();
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
	//_lightColorTexture = new CResource1D(16, LightLUTSize, nullptr);
	//_lightColorTextureTemp = new CResource1D(16, LightLUTSize, nullptr);
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

void Scattering::InitializeInscatteringLUT()
{
	_inscatteringLUT = new CResource3D((int)_inscatteringLUTSize.x, (int)_inscatteringLUTSize.y, (int)_inscatteringLUTSize.z, DXGI_FORMAT_R16G16B16A16_FLOAT);
	_extinctionLUT = new CResource3D((int)_inscatteringLUTSize.x, (int)_inscatteringLUTSize.y, (int)_inscatteringLUTSize.z, DXGI_FORMAT_R16G16B16A16_FLOAT);

	//TODO 랜덤으로 정보를 한번 컴퓨팅 해야될긋 하다

}

void Scattering::PrecomputeSkyboxLUT()
{
	_skyboxLUT = new CResource3D((int)_skyboxLUTSize.x, (int)_skyboxLUTSize.y ,(int)_skyboxLUTSize.z,DXGI_FORMAT_R16G16B16A16_FLOAT);

	//SkyboxLUT compute dispatch;
	precomputeSkyboxLUT->BindShader();
	buffer->SetCSBuffer(2);

	_skyboxLUT->BindResource(0);
	_particleDensityLUT->BindCSShaderResourceView(0); //컴퓨팅으로 대신 그려놨음

	precomputeSkyboxLUT->Dispatch((int)_skyboxLUTSize.x, (int)_skyboxLUTSize.y, (int)_skyboxLUTSize.z);

	_skyboxLUT->ReleaseResource(0);
	_particleDensityLUT->ReleaseCSshaderResorceView(0);
}

void Scattering::UpdateInscatteringLUT()
{

	//InscatteringLUT compute
	buffer->SetCSBuffer(2);
	ScatteringComputeShader->BindShader();
	_camera->Render();

	//bind Textures
	_particleDensityLUT->BindCSShaderResourceView(0);
	_inscatteringLUT->BindResource(1);
	_extinctionLUT->BindResource(2);

	//InscatteringLUT dispatch
	ScatteringComputeShader->Dispatch((int)_inscatteringLUTSize.x, (int)_inscatteringLUTSize.y, 1);

	_particleDensityLUT->ReleaseCSshaderResorceView(0);
	_inscatteringLUT->ReleaseResource(1);
	_extinctionLUT->ReleaseResource(1);
}

D3DXCOLOR Scattering::ComputeLightColor()
{
	
	float cosAngle = D3DXVec3Dot(&D3DXVECTOR3(0, 1, 0), &sun->GetForward());
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
	//TODO 정리해야됨
	buffer->Data.testAmbient = ambient;
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

#include "stdafx.h"
#include "DirectionalLight.h"

#include "../Utilities/Transform.h"
#include "./Renders/WorldBuffer.h"
DirectionalLight::DirectionalLight()
{
	this->sunBuffer = Buffers->FindShaderBuffer<SunBuffer>();
	this->lightViewBuffer = Buffers->FindShaderBuffer<LightViewProj>();

	this->UpdateView();

	D3D11_SAMPLER_DESC desc;
	ZeroMemory(&desc, sizeof D3D11_SAMPLER_DESC);
	desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.MaxAnisotropy = 1;
	Device->CreateSamplerState(&desc, &shadowSampler);
}


DirectionalLight::~DirectionalLight()
{
	SafeDelete(sunBuffer);
	SafeDelete(lightViewBuffer);
}


void DirectionalLight::UpdateView()
{
	D3DXVECTOR3 origin, dir;
	D3DXVECTOR3 center(50, 0, 50);
	origin = sunBuffer->GetPos();
	dir = center - origin;
	D3DXVec3Normalize(&dir, &dir);
	sunBuffer->SetDirection(dir);

	D3DXVECTOR3 up;
	D3DXVec3Cross(&up, &D3DXVECTOR3(1.f, 0.f, 0.f), &dir);

	D3DXMatrixLookAtLH(&view, &origin, &(origin + dir), &D3DXVECTOR3(0.f,1.f,0.f));
	//D3DXMATRIX ortho;
	//D3DXMatrixOrthoLH(&ortho, WinSizeX, WinSizeY, 0.f, 1000.f);

	
	float radius = D3DXVec3Length(&(origin - center));
	float l = center.x - radius;
	float b = center.y - radius;
	float n = center.z - radius;
	float r = center.x + radius;
	float t = center.y + radius;
	float f = center.z + radius;
	
	D3DXMatrixOrthoOffCenterLH(&this->ortho, l, r, b, t, n, f);

	D3DXMATRIX viewProj;
	D3DXMatrixMultiply(&viewProj, &view, &ortho);

	//set special texture matrix for shadow mapping
	float fOffsetX = 0.5f + (0.5f / (float)WinSizeX);
	float fOffsetY = 0.5f + (0.5f / (float)WinSizeY);


	D3DXMATRIX toViewport = D3DXMATRIX(0.5f, 0.0f, 0.0f, 0.0f,
								0.0f, -0.5f, 0.0f, 0.0f,
								0.0f, 0.0f, 1.0f, 0.0f,
								fOffsetX, fOffsetY, 0.0f, 1.0f);

	D3DXMatrixMultiply(&shadowMatrix, &viewProj, &toViewport);

	this->lightViewBuffer->SetShadowMatrix(shadowMatrix);
	this->lightViewBuffer->SetViewProj(viewProj);

}

void DirectionalLight::SetBuffer()
{
	sunBuffer->SetPSBuffer(4);
	lightViewBuffer->SetVSBuffer(5);
	lightViewBuffer->SetPSBuffer(5);
}


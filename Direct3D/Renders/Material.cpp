#include "stdafx.h"
#include "Material.h"
#include "WorldBuffer.h"


Material::Material()
	:name(L""),number(0),diffuseColor(0.f,0.f,0.f,1.f),specColor(0.f,0.f,0.f,1.f),emissiveColor(0.f,0.f,0.f,1.f),shiness(0.f),
	detailFactor(0.f),buffer(NULL),diffuseMap(NULL),normalMap(NULL),specularMap(NULL),emissiveMap(NULL),detailMap(NULL)
{
	this->buffer = Buffers->FindShaderBuffer<MaterialBuffer>();
}


Material::~Material()
{
	//TODO 추 후 리소스 메니져 생기면 그쪽에서 할당 해제
	SafeRelease(this->diffuseMap);
	SafeRelease(this->specularMap);
	SafeRelease(this->normalMap);
	SafeRelease(this->emissiveMap);
}

void Material::Clone(void ** clone)
{
	class Material* material = new Material;

	material->SetNumber(this->number);
	material->SetName(this->name);

	material->SetDiffuseColor(this->diffuseColor);
	material->SetSpecColor(this->specColor);
	material->SetEmissiveColor(this->emissiveColor);

	material->SetShiness(this->shiness);
	material->SetDetailFactor(this->detailFactor);

	if (this->diffuseMap != NULL)
		material->SetDiffuseMap(this->diffuseMap);

	if (this->specularMap != NULL)
		material->SetSpecularMap(this->specularMap);

	if (this->emissiveMap != NULL)
		material->SetEmissiveMap(this->emissiveMap);

	if (this->normalMap != NULL)
		material->SetNormalMap(this->normalMap);

	if (this->detailMap != NULL)
		material->SetDetailMap(this->detailMap);

	*clone = material;
}

void Material::UpdateBuffer()
{
	this->buffer->Data.Ambient = this->ambientColor;
	this->buffer->Data.Diffuse = this->diffuseColor;
	this->buffer->Data.Specular = this->specColor;
	this->buffer->Data.Emissive = this->emissiveColor;
	this->buffer->Data.Shininess = this->shiness;
	this->buffer->Data.DetailFactor = this->detailFactor;
}

void Material::BindBuffer()
{
	this->buffer->SetPSBuffer(3);

	if (this->diffuseMap)
		DeviceContext->PSSetShaderResources(0,1,&this->diffuseMap);
	if (this->specularMap)
		DeviceContext->PSSetShaderResources(1,1,&this->specularMap);
	if (this->emissiveMap)
		DeviceContext->PSSetShaderResources(2,1,&this->emissiveMap);
	if (this->normalMap)
		DeviceContext->PSSetShaderResources(3,1,&this->normalMap);
	if (this->detailMap)
		DeviceContext->PSSetShaderResources(4,1,&this->detailMap);
}

void Material::UnBindBuffer()
{
	ID3D11ShaderResourceView* nullView[1] = { nullptr };
	if (this->diffuseMap)
		DeviceContext->PSSetShaderResources(0, 1, nullView);
	if (this->specularMap)
		DeviceContext->PSSetShaderResources(1, 1, nullView);
	if (this->emissiveMap)
		DeviceContext->PSSetShaderResources(2, 1, nullView);
	if (this->normalMap)
		DeviceContext->PSSetShaderResources(3, 1, nullView);
	if (this->detailMap)
		DeviceContext->PSSetShaderResources(4, 1, nullView);
}

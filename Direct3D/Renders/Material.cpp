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

void Material::BindBuffer()
{
	this->buffer->SetPSBuffer(3);

	if (this->diffuseMap)
		DeviceContext->PSSetShaderResources(5,1,&this->diffuseMap);
	if (this->specularMap)
		DeviceContext->PSSetShaderResources(6,1,&this->specularMap);
	if (this->emissiveMap)
		DeviceContext->PSSetShaderResources(7,1,&this->emissiveMap);
	if (this->normalMap)
		DeviceContext->PSSetShaderResources(8,1,&this->normalMap);
	if (this->detailMap)
		DeviceContext->PSSetShaderResources(9,1,&this->detailMap);
}

void Material::UnBindBuffer()
{
	if (this->diffuseMap)
		DeviceContext->PSSetShaderResources(5, 1, NULL);
	if (this->specularMap)
		DeviceContext->PSSetShaderResources(6, 1, NULL);
	if (this->emissiveMap)
		DeviceContext->PSSetShaderResources(7, 1, NULL);
	if (this->normalMap)
		DeviceContext->PSSetShaderResources(8, 1, NULL);
	if (this->detailMap)
		DeviceContext->PSSetShaderResources(9, 1, NULL);
}

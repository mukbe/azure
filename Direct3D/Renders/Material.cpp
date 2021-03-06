#include "stdafx.h"
#include "Material.h"
#include "WorldBuffer.h"
#include "./Utilities/ImGuiHelper.h"

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

void Material::UpdateBuffer()
{
	this->buffer->Data.Ambient = this->ambientColor;
	this->buffer->Data.Diffuse = this->diffuseColor;
	this->buffer->Data.Specular = this->specColor;
	this->buffer->Data.Emissive = this->emissiveColor;
	this->buffer->Data.Shininess = this->shiness;
	this->buffer->Data.DetailFactor = this->detailFactor;
	
	this->buffer->Data.textureCheck = 0;
	if (diffuseMap)
		this->buffer->Data.textureCheck |= UseDeiffuseMap;
	if (normalMap)
		this->buffer->Data.textureCheck |= UseNormalMap;
	if (specularMap)
		this->buffer->Data.textureCheck |= UseSpecularMap;
	if (emissiveMap)
		this->buffer->Data.textureCheck |= UseEmissiveMap;
}

void Material::BindBuffer()
{
	this->buffer->SetVSBuffer(3);
	this->buffer->SetPSBuffer(3);

	ID3D11ShaderResourceView* view;
	if (this->diffuseMap)
	{
		view = diffuseMap->GetSRV();
		DeviceContext->PSSetShaderResources(0, 1, &view);
	}
	if (this->specularMap)
	{
		view = specularMap->GetSRV();
		DeviceContext->PSSetShaderResources(1, 1, &view);
	}
	
	if (this->emissiveMap) 
	{
		view = emissiveMap->GetSRV();
		DeviceContext->PSSetShaderResources(2, 1, &view);
	}
	if (this->normalMap)
	{
		view = normalMap->GetSRV();
		DeviceContext->PSSetShaderResources(3, 1, &view);
	}
	if (this->detailMap)
	{
		view = detailMap->GetSRV();
		DeviceContext->PSSetShaderResources(4, 1, &view);
	}
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

void Material::UIRender()
{
	ImGuiHelper::RenderImageButton(&diffuseMap);
	ImGui::SameLine();
	ImGui::Text("DiffuseMap");

	ImGuiHelper::RenderImageButton(&specularMap);
	ImGui::SameLine();
	ImGui::Text("SpecularMap");

	ImGuiHelper::RenderImageButton(&emissiveMap);
	ImGui::SameLine();
	ImGui::Text("EmissiveMap");

	ImGuiHelper::RenderImageButton(&normalMap);
	ImGui::SameLine();
	ImGui::Text("NormalMap");

	ImGuiHelper::RenderImageButton(&detailMap);
	ImGui::SameLine();
	ImGui::Text("DetailMap");

	ImGui::Separator();

	ImGui::ColorEdit4("AmbientColor", (float*)&ambientColor.r,
		ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaPreviewHalf);

	ImGui::ColorEdit4("DiffuseColor", (float*)&diffuseColor.r,
		ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaPreviewHalf);

	ImGui::ColorEdit4("SpecColor", (float*)&specColor.r,
		ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaPreviewHalf);

	ImGui::ColorEdit4("EmissiveColor", (float*)&emissiveColor.r,
		ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaPreviewHalf);

	ImGui::InputFloat("Shiness", &shiness);

	ImGui::Separator();

	UpdateBuffer();
}

void Material::SaveData(Json::Value * parent)
{
	
	
}

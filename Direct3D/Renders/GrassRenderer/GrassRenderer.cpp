#include "stdafx.h"
#include "GrassRenderer.h"

#include "./Utilities/Buffer.h"
#include "./Utilities/ImGuiHelper.h"

GrassRenderer::GrassRenderer(UINT maxGrass)
	:maxGrassCount(maxGrass), currentTexture(nullptr)
{
	this->name = "GrassRenderer";

	shader = new Shader(ShaderPath + L"008_Grass.hlsl",Shader::ShaderType::useGS);

	this->grassList.push_back(GrassData(D3DXVECTOR3(), D3DXVECTOR2(), D3DXVECTOR3(), UINT()));
	this->CreateBuffer();
}

GrassRenderer::~GrassRenderer()
{
}

void GrassRenderer::Init()
{	
	this->textureList.push_back(AssetManager->FindTexture("grass00"));
	this->textureList.push_back(AssetManager->FindTexture("grass01"));
	this->textureList.push_back(AssetManager->FindTexture("grass02"));

	this->currentTexture = textureList[0];
}

void GrassRenderer::Release()
{
	SafeRelease(grassBuffer);
	
	grassList.clear();
	textureList.clear();
}

void GrassRenderer::PreUpdate()
{
}

void GrassRenderer::Update()
{
}

void GrassRenderer::PostUpdate()
{
}

void GrassRenderer::PrevRender()
{
}

void GrassRenderer::Render()
{
	//Bind ---------------------------------------------------------------
	//States::SetRasterizer(States::RasterizerStates::SOLID_CULL_OFF);
	//
	//ID3D11ShaderResourceView* view[5];
	//UINT i;
	//for (i = 0; i < textureList.size(); ++i)
	//{
	//	view[i] = textureList[i]->GetSRV();
	//}
	//// -------------------------------------------------------------------
	//
	//DeviceContext->PSSetShaderResources(0, textureList.size(), view);
	//
	//UINT stride = sizeof GrassData;
	//UINT offset = 0;
	//
	//DeviceContext->IASetVertexBuffers(0, 1, &grassBuffer, &stride, &offset);
	//DeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
	//DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	//
	//shader->Render();
	//
	//DeviceContext->Draw(grassList.size(), 0);
	//
	//shader->ReleaseShader();
	//
	//States::SetRasterizer(States::RasterizerStates::SOLID_CULL_ON);
}

void GrassRenderer::UIUpdate()
{
	
}

void GrassRenderer::UIRender()
{
	static D3DXVECTOR2 scale = D3DXVECTOR2(1.f, 1.f);

	ID3D11ShaderResourceView* view = currentTexture->GetSRV();
	if(ImGui::BeginCombo("List", String::WStringToString(currentTexture->GetFile()).c_str()))
	{
		for (UINT i = 0; i < textureList.size(); ++i)
		{
			bool is_selected = (textureList[i]->GetFile() == currentTexture->GetFile());
			if (ImGui::Selectable(String::WStringToString(textureList[i]->GetFile()).c_str(), is_selected))
				currentTexture = textureList[i];
			if (is_selected);
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::Image(view, ImVec2(100, 100));
}

void GrassRenderer::ShadowRender()
{
}

void GrassRenderer::AddGrass(GrassData data)
{
	if (maxGrassCount <= grassList.size())
		return;

	grassList.push_back(data);
	this->UpdateBuffer();
}

void GrassRenderer::CreateBuffer(bool mapTool /*== true*/)
{
	if (mapTool)
		Buffer::CreateDynamicVertexBuffer(&grassBuffer, grassList.data(), sizeof GrassData *  maxGrassCount);
	else
		Buffer::CreateVertexBuffer(&grassBuffer, grassList.data(), sizeof GrassData * grassList.size());
}

void GrassRenderer::UpdateBuffer()
{
	Buffer::UpdateBuffer(&grassBuffer, grassList.data(), sizeof GrassData * grassList.size());
}

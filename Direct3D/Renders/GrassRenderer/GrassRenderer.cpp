#include "stdafx.h"
#include "GrassRenderer.h"

#include "./Utilities/Buffer.h"
#include "./Utilities/BinaryFile.h"
#include "./Utilities/ImGuiHelper.h"
#include "./Project/TerrainTool/Terrain.h"
#include "./Object/GameObject/TagMessage.h"

GrassRenderer::GrassRenderer(UINT maxGrass)
	:maxGrassCount(maxGrass), currentTexture(nullptr), terrain(nullptr), currentTextureIndex(0)
{
	this->name = "GrassRenderer";

	shader = new Shader(ShaderPath + L"008_Grass.hlsl",Shader::ShaderType::useGS);
	shadowShader = new Shader(ShaderPath + L"008_Grass.hlsl", Shader::ShaderType::useGS, "GrassShadow");

	this->grassList.push_back(GrassData(D3DXVECTOR3(-10000,-10000,10000), D3DXVECTOR2(0.1f,0.1f), D3DXVECTOR3(0,0,1), UINT(0)));
	this->CreateBuffer(true);
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
	SafeDelete(shader);
	SafeDelete(shadowShader);

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
	if (grassList.size() >= 2)
	{
		//Bind ---------------------------------------------------------------
		ID3D11ShaderResourceView* tempView[5] = { nullptr,nullptr,nullptr,nullptr,nullptr };
		for (size_t i = 0; i < textureList.size(); ++i)
			tempView[i] = textureList[i]->GetSRV();

		States::SetRasterizer(States::RasterizerStates::SOLID_CULL_OFF);

		// -------------------------------------------------------------------

		DeviceContext->PSSetShaderResources(0, textureList.size(), tempView);

		UINT stride = sizeof GrassData;
		UINT offset = 0;

		DeviceContext->IASetVertexBuffers(0, 1, &grassBuffer, &stride, &offset);
		DeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
		DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		shader->Render();

		DeviceContext->Draw(grassList.size(), 0);

		shader->ReleaseShader();

		States::SetRasterizer(States::RasterizerStates::SOLID_CULL_ON);
	}
}

void GrassRenderer::ShadowRender()
{
	if (grassList.size() >= 2)
	{

		//Bind ---------------------------------------------------------------
		ID3D11ShaderResourceView* tempView[5] = { nullptr,nullptr,nullptr,nullptr,nullptr };
		for (size_t i = 0; i < textureList.size(); ++i)
			tempView[i] = textureList[i]->GetSRV();

		States::SetRasterizer(States::RasterizerStates::SHADOW);

		// -------------------------------------------------------------------
		DeviceContext->PSSetShaderResources(0, textureList.size(), tempView);

		UINT stride = sizeof GrassData;
		UINT offset = 0;

		DeviceContext->IASetVertexBuffers(0, 1, &grassBuffer, &stride, &offset);
		DeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
		DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		shadowShader->Render();

		DeviceContext->Draw(grassList.size(), 0);

		shadowShader->ReleaseShader();
		States::SetRasterizer(States::RasterizerStates::SHADOW);
	}
}

void GrassRenderer::SaveData(Json::Value * parent)
{
	string file = "../_Scenes/Scene01/" + name + ".bin";
	Json::Value value;
	{
		JsonHelper::SetValue(value, "Name", this->name);
		JsonHelper::SetValue(value, "FileName", file);
		JsonHelper::SetValue(value, "IsActive", isActive);
	}
	(*parent)[this->name.c_str()] = value;


	BinaryWriter* w = new BinaryWriter;
	w->Open(String::StringToWString(file));
	{
		w->UInt(grassList.size());

		for (size_t i = 0; i < grassList.size(); ++i)
		{
			w->Vector3(grassList[i].position);
			w->Vector2(grassList[i].scale);
			w->Vector3(grassList[i].normal);
			w->UInt(grassList[i].textureID);
			
		}
	}
	w->Close();
	SafeDelete(w);
}

void GrassRenderer::LoadData(Json::Value * parent)
{
	string file;
	JsonHelper::GetValue(*parent, "IsActive", isActive);
	JsonHelper::GetValue(*parent, "FileName", file);

	BinaryReader* r = new BinaryReader;
	r->Open(String::StringToWString(file));
	{
		UINT size = r->UInt();

		for (size_t i = 0; i < size; ++i)
		{
			GrassData newData;
			newData.position = r->Vector3();
			newData.scale = r->Vector2();
			newData.normal = r->Vector3();
			newData.textureID = r->UInt();
			this->AddGrass(newData);
		}
	}
	r->Close();
	SafeDelete(r);
}



void GrassRenderer::UIUpdate()
{
	if (KeyCode->Press(VK_CONTROL) && KeyCode->Down(VK_LBUTTON))
	{
		if (terrain)
		{
			D3DXVECTOR3 pickPos = terrain->PickPos();
			createData.position = pickPos;
			createData.textureID = currentTextureIndex;
			createData.normal = D3DXVECTOR3(0, 0, 1);
			this->AddCrossGrass(createData);
		}
	}
	
}

void GrassRenderer::UIRender()
{
	if (ImGui::CollapsingHeader("Info"))
	{
		ImGui::Text("GrassNum : %d", grassList.size() - 1);
	}

	static D3DXVECTOR2 scale = D3DXVECTOR2(1.f, 1.f);
	if (ImGui::CollapsingHeader("Image"))
	{
		ID3D11ShaderResourceView* view = currentTexture->GetSRV();
		if (ImGui::BeginCombo("List", String::WStringToString(currentTexture->GetFile()).c_str()))
		{
			for (UINT i = 0; i < textureList.size(); ++i)
			{
				bool is_selected = (textureList[i]->GetFile() == currentTexture->GetFile());
				if (ImGui::Selectable(String::WStringToString(textureList[i]->GetFile()).c_str(), is_selected))
				{
					currentTexture = textureList[i];
					currentTextureIndex = i;
				}
				if (is_selected);
				ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::Image(view, ImVec2(100, 100));
	}

	if (ImGui::CollapsingHeader("Create"))
	{
		if (ImGui::Button("LinkTerrain"))
			this->LinkTerrain();
		ImGui::SameLine();
		if (ImGui::Button("ResetData"))
			this->ResetData();
		
		ImGui::SliderFloat2("GrassScale", &createData.scale.x, 0.1, 10.0f);
	}



}


void GrassRenderer::AddCrossGrass(GrassData data)
{
	float angle = 0.f;
	float angleRatio = D3DX_PI / (float)2.0f;
	for (int i = 0; i < 2; ++i)
	{
		D3DXMATRIX rotateY;
		D3DXMatrixRotationY(&rotateY, angle);
		D3DXVECTOR3 look(0.f, 0.f, 1.f);
		D3DXVec3TransformNormal(&look, &look, &rotateY);

		data.normal = look;

		this->AddGrass(data);

		angle += angleRatio;
	}
}

void GrassRenderer::AddGrass(GrassData data)
{
	if (maxGrassCount <= grassList.size())
		return;

	grassList.push_back(data);
	this->UpdateBuffer();
}

void GrassRenderer::LinkTerrain()
{
	terrain = (Terrain*)Objects->FindObject(ObjectType::Type::Dynamic, ObjectType::Tag::Enviroment, "Terrain");
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

void GrassRenderer::ResetData()
{
	grassList.clear();
	this->grassList.push_back(GrassData(D3DXVECTOR3(-10000, -10000, 10000), D3DXVECTOR2(0.1f, 0.1f), D3DXVECTOR3(0, 0, 1), UINT(0)));
	this->UpdateBuffer();
}

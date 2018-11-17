#include "stdafx.h"
#include "GameUnit.h"

#include "./Model/Model.h"
#include "./Model/ModelAnimPlayer.h"
#include "./Model/ModelAnimClip.h"
#include "../../Bounding/GameCollider.h"
#include "./Bounding/AnimationCollider.h"
#include "./Object/GameObject/TagMessage.h"
#include "./Renders/Material.h"

GameUnit::GameUnit(string name,class Model* model)
	:GameObject(name),model(model)
{
	this->animation = new ModelAnimPlayer(model);
}

GameUnit::GameUnit(string name)
	:GameObject(name)
{
	ModelData data = AssetManager->GetModelData(name);
	this->model = new Model;
	data.Clone(model);
	model->BindMeshData();

	this->animation = new ModelAnimPlayer(model);
	this->animation->SetTransform(this->transform);

	for (UINT i = 0; i < data.colliders.size(); ++i)
	{
		AnimationCollider* newCollider = nullptr;
		((AnimationCollider*)data.colliders[i])->Clone((void**)&newCollider);
		newCollider->SetParentObject(this);
		newCollider->SetAnimation(this->animation);
		this->colliderList.push_back(newCollider);
	}

	this->AddCallback("Delete", [this](TagMessage msg) 
	{
		this->isLive = false;
	});
}


GameUnit::~GameUnit()
{
	SafeDelete(model);
	SafeDelete(animation);

	for (UINT i = 0; i < colliderList.size(); ++i)
		SafeDelete(colliderList[i]);
	colliderList.clear();
}

void GameUnit::Init()
{
}

void GameUnit::Release()
{
}

void GameUnit::PreUpdate()
{
}

void GameUnit::Update()
{
	this->animation->Update();

	for (UINT i = 0; i < colliderList.size(); ++i)
		colliderList[i]->Update();
}

void GameUnit::PostUpdate()
{
}

void GameUnit::PrevRender()
{
}

void GameUnit::ShadowRender()
{
	this->animation->ShadowRender();
}

void GameUnit::Render()
{
	this->animation->Render();

	if (ObjectManager::Get()->GetIsDebug())
	{
		for (UINT i = 0; i < colliderList.size(); ++i)
			colliderList[i]->Render();
	}
}

void GameUnit::UIRender()
{
	if (ImGui::Button("SaveMaterial", ImVec2(100, 20)))
		this->SaveMaterial(L"");

	if (ImGui::CollapsingHeader("Material"))
	{
		if(model)
			model->UIRender();
	}
	if (ImGui::CollapsingHeader("Animation"))
	{
		static float blendTime = 0.2f;
		ImGui::SliderFloat("BlendTime", &blendTime, 0.0f, 1.0f);
		vector<ModelAnimClip*> clips = model->GetClips();
		static wstring targetName = clips[0]->Name();
		if (ImGui::BeginCombo("List", String::WStringToString(targetName).c_str()))
		{
			for (UINT i = 0; i < clips.size(); ++i)
			{
				wstring name = clips[i]->Name();
				if (ImGui::Selectable(String::WStringToString(name).c_str()))
				{
					targetName = clips[i]->Name();
					animation->ChangeAnimation(targetName, blendTime);
					ImGui::SetItemDefaultFocus();
				}
					
			}

			ImGui::EndCombo();
		}

	}
}

void GameUnit::SaveData(Json::Value * parent)
{

}

void GameUnit::LoadData(Json::Value * parent)
{

}

void GameUnit::SaveMaterial(wstring file)
{
	if (file.length() > 0)
	{
		Json::Value root;
		vector<Material*> materials = model->MaterialS();
		for (UINT i = 0; i < materials.size(); ++i)
		{
			Material* material = materials[i];

			Json::Value val;
			JsonHelper::SetValue(val, "Name", String::WStringToString(material->GetName()));

			JsonHelper::SetValue(val, "Ambient", material->GetAmbientColor());
			JsonHelper::SetValue(val, "Diffuse", material->GetDiffuseColor());
			JsonHelper::SetValue(val, "Emissive", material->GetEmissiveColor());
			JsonHelper::SetValue(val, "Specular", material->GetSpecColor());
			float f = material->GetShiness();
			JsonHelper::SetValue(val, "Shininess", f);

			string nullString = "";
			string file = "";

			if (material->GetDiffuseMap())
				file = String::WStringToString(material->GetDiffuseMap()->GetFilePath());
			else
				file = nullString;

			JsonHelper::SetValue(val, "DiffuseFile", file);

			if (material->GetSpecularMap())
				file = String::WStringToString(material->GetSpecularMap()->GetFilePath());
			else
				file = nullString;

			JsonHelper::SetValue(val, "SpecularFile", nullString);

			JsonHelper::SetValue(val, "EmissiveFile", nullString);

			if (material->GetNormalMap())
				file = String::WStringToString(material->GetNormalMap()->GetFilePath());
			else
				file = nullString;

			JsonHelper::SetValue(val, "NormalFile", file);

			JsonHelper::SetValue(val, "DetailFile", nullString);

			root[String::WStringToString(material->GetName()).c_str()] = val;
		}

		JsonHelper::WriteData(file, &root);
	}
	else
	{
		function<void(wstring)> func = bind(&GameUnit::SaveMaterial, this, placeholders::_1);
		Path::SaveFileDialog(L"", Path::MaterialFilter, Assets, func);
	}
}



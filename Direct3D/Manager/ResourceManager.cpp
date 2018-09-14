#include "stdafx.h"
#include "ResourceManager.h"

#include "./Model/Model.h"
#include "./Model/ModelAnimClip.h"
#include "./Model/ModelBone.h"
#include "./Model/ModelMesh.h"

#include "./Renders/Material.h"

SingletonCpp(ResourceManager)

ResourceManager::ResourceManager()
	:isShow(true)
{

}
ResourceManager::~ResourceManager()
{
	TextureIter iter = textures.begin();
	for (; iter != textures.end(); ++iter)
		SafeDelete(iter->second);
	textures.clear();

	ModelContainerIter modelIter = models.begin();
	for (; modelIter != models.end(); ++modelIter)
	{
		modelIter->second.Release();
	}
	models.clear();
}


void ResourceManager::LoadAsset()
{
	this->AddModelData(Assets + L"Pandaren/Pandaren", true);
	this->AddModelData(Assets + L"Objects/FishingBox/FishingBox");
	this->AddModelData(Assets + L"Objects/Trees/Tree1");
	this->AddModelData(Assets + L"Objects/Trees/Tree2");
	this->AddModelData(Assets + L"Objects/Trees/Tree3");
	this->AddModelData(Assets + L"Objects/Trees/Tree4");
	this->AddModelData(Assets + L"Objects/Trees/Tree5");
}

Texture * ResourceManager::AddTexture(wstring file, string keyName)
{
	TextureIter iter = textures.find(keyName);
	if (iter != textures.end())
		return iter->second;

	Texture* newTexture = new Texture(file);
	this->textures.insert(make_pair(keyName, newTexture));

	return newTexture;
}

Texture * ResourceManager::FindTexture(string keyName)
{
	TextureIter iter = textures.find(keyName);
	if (iter == textures.end())return nullptr;
	
	return iter->second;
}

ID3D11ShaderResourceView * ResourceManager::GetRsourceView(string keyName)
{
	TextureIter iter = textures.find(keyName);
	if (iter == textures.end())return nullptr;
	return iter->second->GetSRV();
}

ModelData ResourceManager::AddModelData(wstring file, string keyName,bool isAnimation)
{
	ModelContainerIter iter = models.find(keyName);
	if (iter != models.end())
		return iter->second;

	ModelData data;

	Models::LoadMaterial(file + L".material", &data.materials);
	Models::LoadMesh(file + L".mesh", &data.meshDatas.Bones, &data.meshDatas.Meshes);

	if(isAnimation)
		Models::LoadAnimation(file + L".anim", &data.animations);

	this->models.insert(make_pair(keyName, data));

	return data;
}

ModelData ResourceManager::AddModelData(wstring file, bool isAnimation)
{
	wstring keyName = Path::GetFileNameWithoutExtension(file);
	return AddModelData(file, String::WStringToString(keyName), isAnimation);
}

ModelData ResourceManager::GetModelData(string keyName)
{
	ModelContainerIter iter = models.find(keyName);
	if (iter != models.end())
	{
		return iter->second;
	}

	return ModelData();
}

Model * ResourceManager::GetModel(wstring file, string keyName, bool isAnimation)
{
	ModelContainerIter iter = models.find(keyName);
	if (iter != models.end())
	{
		ModelData data = iter->second;
		class Model* model = new Model;
		data.Clone(model);
		model->BindMeshData();
		return model;
	}

	ModelData data = this->AddModelData(file, keyName, isAnimation);
	Model* model = new Model;
	data.Clone(model);
	model->BindMeshData();

	return model;
}

Model * ResourceManager::GetModel(wstring file, bool isAnimation)
{
	wstring keyName = Path::GetFileNameWithoutExtension(file);

	return this->GetModel(file, String::WStringToString(keyName), isAnimation);
}



void ResourceManager::UIRender()
{
	if (isShow == false) return;

	ImGuiDragDropFlags src_flags = 0;
	src_flags |= ImGuiDragDropFlags_SourceNoDisableHover;
	src_flags |= ImGuiDragDropFlags_SourceNoHoldToOpenOthers;

	ImGui::Begin("Assets");
	{
		//Textures ----------------------------------------------------------------------------
		if (ImGui::TreeNode("Textures"))
		{
			TextureIter iter = textures.begin();
			for (; iter != textures.end(); ++iter)
			{
				ImGui::Selectable(iter->first.c_str());

				if (ImGui::BeginDragDropSource(src_flags))
				{
					if (!(src_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
					{
						ImGui::Text("%s", iter->first.c_str());
						ImGui::Image(iter->second->GetSRV(), ImVec2(50, 50));
					}
					ImGui::SetDragDropPayload("TextureMove", iter->second, sizeof(Texture));
					ImGui::EndDragDropSource();
				}
			}

			ImGui::TreePop();
		}
		//--------------------------------------------------------------------------------------
		//Models ----------------------------------------------------------------------------
		if (ImGui::TreeNode("Models"))
		{
			ModelContainerIter iter = models.begin();
			for (; iter != models.end(); ++iter)
			{
				ImGui::Selectable(iter->first.c_str());

				if (ImGui::BeginDragDropSource(src_flags))
				{
					if (!(src_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
					{
						ImGui::Text("%s", iter->first.c_str());
					}
					ImGui::SetDragDropPayload("ModelMove", &iter->second, sizeof(ModelData));
					ImGui::EndDragDropSource();
				}
			}
			ImGui::TreePop();
		}

	}
	ImGui::End();
}




void ModelData::Clone(vector<class Material*>* pMaterials, vector<class ModelBone*>* pBones, 
	vector<class ModelMesh*>* pMeshes, vector<class ModelAnimClip*>* pAnim)
{
	pMaterials->clear();
	for (Material* material : materials)
	{
		Material* temp = nullptr;
		material->Clone((void **)&temp);

		pMaterials->push_back(temp);
	}

	pBones->clear();
	for (ModelBone* bone : meshDatas.Bones)
	{
		ModelBone* temp = nullptr;
		bone->Clone((void**)&temp);

		pBones->push_back(temp);
	}

	pMeshes->clear();
	for (ModelMesh* mesh : meshDatas.Meshes)
	{
		ModelMesh* temp = nullptr;
		mesh->Clone((void**)&temp);

		pMeshes->push_back(temp);
	}

	pAnim->clear();
	for (ModelAnimClip* clip : animations)
	{
		ModelAnimClip* temp = nullptr;
		clip->Clone((void**)&temp);

		pAnim->push_back(temp);
	}
}

void ModelData::Clone(Model * model)
{
	this->Clone(&model->materials, &model->bones, &model->meshes, &model->clips);
}

void ModelData::Release()
{
	for (Material* material : materials)
	{
		SafeDelete(material)
	}
	materials.clear();

	for (ModelBone* bone : meshDatas.Bones)
	{
		SafeDelete(bone);
	}
	meshDatas.Bones.clear();

	for (ModelMesh* mesh : meshDatas.Meshes)
	{
		SafeDelete(mesh);
	}

	for (ModelAnimClip* clip : animations)
	{
		SafeDelete(clip);
	}
	animations.clear();
}

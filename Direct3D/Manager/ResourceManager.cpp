#include "stdafx.h"
#include "ResourceManager.h"

#include "./Model/Model.h"
#include "./Model/ModelAnimClip.h"
#include "./Model/ModelBone.h"
#include "./Model/ModelMesh.h"

#include "./Renders/Material.h"

#include "./Bounding/GameCollider.h"
#include "./Bounding/AnimationCollider.h"
#include "./Bounding/BoundingBox.h"

#include "./Utilities/BinaryFile.h"


#include <io.h>


SingletonCpp(ResourceManager)

ResourceManager::ResourceManager()
	:isShow(true)
{
	Texture* nullTexture = new Texture;
	textures.insert(make_pair("NullTexture", nullTexture));
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
	this->LoadFolder("../Contents/Atmospheric/","*.png");
	this->LoadFolder("../_Contents/", "*.png");

	this->LoadFolder("../_Assets/Objects/Trees/", "*.png");
	this->LoadFolder("../_Assets/Objects/Trees/", "*.material");
	this->LoadFolder("../_Assets/Objects/FishingBox/", "*.png");
	this->LoadFolder("../_Assets/Objects/FishingBox/", "*.material");
	this->LoadFolder("../_Assets/Objects/Blacksmeeth/", "*.png");
	this->LoadFolder("../_Assets/Objects/Blacksmeeth/", "*.material");

	this->LoadFolder("../_Assets/Bard2/", "*.material", true);
	this->LoadFolder("../_Assets/Bard2/", "*.png");
	this->LoadFolder("../_Assets/Pandaren/", "*.material",true);
	this->LoadFolder("../_Assets/Pandaren/", "*.png");
}

void ResourceManager::LoadFolder(const std::string& path, const std::string& filter, bool isAnim)
{
	std::string searching = path + filter;

	std::vector<std::string> return_;

	_finddata_t fd;
	long handle = _findfirst(searching.c_str(), &fd);  //현재 폴더 내 모든 파일을 찾는다.

	if (handle == -1)return;

	int result = 0;
	do
	{
		string filePath = path + fd.name;

		if (filter == "*.png")
			this->AddTexture(String::StringToWString(filePath), Path::GetFileNameWithoutExtension(fd.name));
		else if (filter == "*.material")
		{
			if(isAnim)
				this->AddModelData(Path::GetFilePathWithoutExtension(String::StringToWString(path + fd.name)), true);
			else
				this->AddModelData(Path::GetFilePathWithoutExtension(String::StringToWString(path + fd.name)), false);
		}

		result = _findnext(handle, &fd);
	} while (result != -1);

	_findclose(handle);
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

Texture * ResourceManager::AddTexture(string key, Texture * texture)
{
	TextureIter iter = textures.find(key);
	if (iter != textures.end())
		return iter->second;

	this->textures.insert(make_pair(key, texture));

	return texture;
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
	data.file = file;
	Models::LoadMaterial(file + L".material", &data.materials);
	Models::LoadMesh(file + L".mesh", &data.meshDatas.Bones, &data.meshDatas.Meshes);

	if (isAnimation)
	{
		Models::LoadAnimation(file + L".anim", &data.animations);

		BinaryReader* reader = new BinaryReader();
		reader->Open(file + L".collider");
		{
			UINT size = reader->UInt();
			for (UINT i = 0; i < size; ++i)
			{
				AnimationCollider* collider = new AnimationCollider(nullptr,nullptr);
				GameCollider::LoadAnimCollider(reader, collider);
				data.colliders.push_back(collider);
			}
		}
		reader->Close();
		SafeDelete(reader);
	}
	else
	{
		BinaryReader* reader = new BinaryReader();
		if(reader->Open(file + L".collider"))
		{
			UINT size = reader->UInt();
			for (UINT i = 0; i < size; ++i)
			{
				GameCollider* collider = new GameCollider(nullptr, new BoundingBox());
				GameCollider::LoadCollider(reader, collider);
				data.colliders.push_back(collider);
			}
			reader->Close();
		}
		SafeDelete(reader);
	}

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
		ImGui::BeginGroup();
		if (ImGui::TreeNode("Textures"))
		{
			ImVec2 winSize = ImGui::GetWindowSize();
			
			float startOffset = 200.0f;
			float currentX = startOffset;

			TextureIter iter = textures.begin();
			for (; iter != textures.end(); ++iter)
			{
				ImGui::ImageButton(iter->second->GetSRV(), ImVec2(50,50));

				if (currentX < winSize.x - 10.0f)
				{
					currentX += (60.0f);
					ImGui::SameLine();
				}
				else
					currentX = startOffset;

				if (ImGui::BeginDragDropSource(src_flags))
				{
					if (!(src_flags & ImGuiDragDropFlags_SourceNoPreviewTooltip))
					{
						ImGui::Text("%s", iter->first.c_str());
						ImGui::Image(iter->second->GetSRV(), ImVec2(50, 50));
					}
					ImGui::SetDragDropPayload("TextureMove", &iter->second, sizeof(UINT));
					ImGui::EndDragDropSource();
				}
			}

			ImGui::TreePop();
		}
		ImGui::EndGroup();
		//--------------------------------------------------------------------------------------
		//Models ----------------------------------------------------------------------------
		ImGui::BeginGroup();
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
		ImGui::EndGroup();

	}
	ImGui::End();
}




void ModelData::Clone(vector<class Material*>* pMaterials, vector<class ModelBone*>* pBones, 
	vector<class ModelMesh*>* pMeshes, vector<class ModelAnimClip*>* pAnim, vector<class GameCollider*>* colliders)
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

	if (pAnim)
	{
		pAnim->clear();
		for (ModelAnimClip* clip : animations)
		{
			ModelAnimClip* temp = nullptr;
			clip->Clone((void**)&temp);

			pAnim->push_back(temp);
		}
	}

	if (colliders)
	{
		colliders->clear();
		for (GameCollider* collider : this->colliders)
		{
			GameCollider* newCollider = nullptr;
			collider->Clone((void**)&newCollider);
			colliders->push_back(newCollider);
		}
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

	for (GameCollider* collider : colliders)
		SafeDelete(collider);

	colliders.clear();
}

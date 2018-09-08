#include "stdafx.h"
#include "Model.h"


#include "ModelMesh.h"
#include "ModelBone.h"
#include "ModelMeshPart.h"
#include "ModelAnimClip.h"
#include "../Utilities/BinaryFile.h"
#include "../Renders/Material.h"
#include "../Utilities/String.h"
#include "../Utilities/Path.h"
#include "../Utilities/Buffer.h"
#include <unordered_map>

void Model::ReadMaterial(wstring file)
{
	Models::LoadMaterial(file, &materials);
}

void Model::ReadMesh(wstring file)
{
	Models::LoadMesh(file, &bones, &meshes);

	BindMeshData();
}

void Model::ReadAnimation(wstring file)
{
	Models::LoadAnimation(file, &clips);
}

Material * Model::GetMaterialByName(wstring name)
{
	for (Material* material : materials)
	{
		if (material->GetName() == name)
			return material;
	}

	return NULL;
}

void Model::BindMeshData()
{
	for (ModelMesh* mesh : meshes)
	{
		for (ModelMeshPart* part : mesh->meshParts)
			part->material = GetMaterialByName(part->materialName);
	}


	this->root = bones[0];
	for (ModelBone* bone : bones)
	{
		if (bone->parentIndex > -1)
		{
			bone->parent = bones[bone->parentIndex];
			bone->parent->childs.push_back(bone);
		}
		else
			bone->parent = nullptr;

	}

	for (size_t i = 0; i < meshes.size(); i++)
	{
		meshes[i]->parentBone = bones[meshes[i]->parentBoneIndex];
		meshes[i]->Binding();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void Models::Create()
{
}

void Models::Delete()
{
	for (pair<wstring, vector<Material*>> temp : materialMap)
	{
		for (Material* material : temp.second)
			SafeDelete(material);
	}

	for (pair<wstring, MeshData> temp : meshDataMap)
	{
		MeshData data = temp.second;

		for (ModelBone* bone : data.Bones)
			SafeDelete(bone);

		data.Bones.clear();

		for (ModelMesh* mesh : data.Meshes)
			SafeDelete(mesh);

		data.Meshes.clear();
	}

	materialMap.clear();
	meshDataMap.clear();
}


map<wstring, vector<class Material *>> Models::materialMap;
void Models::LoadMaterial(wstring file, vector<class Material*> * materials)
{
	if (materialMap.count(file) < 1)
	{
		vector<Material *> srcMaterials;

		Json::Value root;
		JsonHelper::ReadData(file, &root);

		Json::Value::iterator iter = root.begin();
		for (; iter != root.end(); iter++)
		{
			Json::Value value = (*iter);
			Material* material = new Material();

			string name;
			JsonHelper::GetValue(value, "Name", name);
			material->SetName(String::StringToWString(name));

			D3DXCOLOR color;
			JsonHelper::GetValue(value, "Ambient", color);
			material->SetAmbientColor(color);

			JsonHelper::GetValue(value, "Diffuse", color);
			material->SetDiffuseColor(color);

			JsonHelper::GetValue(value, "Emissive", color);
			material->SetEmissiveColor(color);

			JsonHelper::GetValue(value, "Specular", color);
			material->SetSpecColor(color);

			float shininess;
			JsonHelper::GetValue(value, "Shininess", shininess);
			material->SetShiness(shininess);

			string textureFile;
			string directory = "";//Path::GetDirectoryName(StringHelper::WStringToString(file));

			JsonHelper::GetValue(value, "DiffuseFile", textureFile);
			if (textureFile.length() > 0)
			{
				ID3D11ShaderResourceView* resource = NULL;
				Buffer::CreateTexture(String::StringToWString(textureFile), &resource);
				material->SetDiffuseMap(resource);
			}
			
			//TODO 추후 자원관리 클래스 나오면 작업.
			//JsonHelper::GetValue(value, "SpecularFile", textureFile);
			//if (textureFile.length() > 0)
			//	material->SetSpecularMap(directory + textureFile);
			
			//JsonHelper::GetValue(value, "EmissiveFile", textureFile);
			//if (textureFile.length() > 0)
			//	material->SetEmissiveMap(directory + textureFile);
			
			JsonHelper::GetValue(value, "NormalFile", textureFile);
			if (textureFile.length() > 0)
			{
				ID3D11ShaderResourceView* resource = NULL;
				Buffer::CreateTexture(String::StringToWString(textureFile), &resource);
				material->SetNormalMap(resource);
			}

		

			srcMaterials.push_back(material);
		}//if(materialMap)

		materialMap[file] = srcMaterials;
		srcMaterials.clear();
	}

	materials->clear();
	for (Material* material : materialMap[file])
	{
		Material* temp = NULL;
		material->Clone((void **)&temp);

		materials->push_back(temp);
	}
}

map<wstring, Models::MeshData> Models::meshDataMap;
void Models::LoadMesh(wstring file, vector<ModelBone*>* bones, vector<ModelMesh*>* meshes)
{
	if (meshDataMap.count(file) < 1)
		ReadMeshData(file);

	MeshData data = meshDataMap[file];
	for (size_t i = 0; i < data.Bones.size(); i++)
	{
		ModelBone* bone = NULL;
		data.Bones[i]->Clone((void **)&bone);

		bones->push_back(bone);
	}

	for (size_t i = 0; i < data.Meshes.size(); i++)
	{
		ModelMesh* mesh = NULL;
		data.Meshes[i]->Clone((void **)&mesh);

		meshes->push_back(mesh);
	}
}

void Models::ReadMeshData(wstring file)
{
	BinaryReader* r = new BinaryReader();
	r->Open(file);

	UINT count = 0;

	count = r->UInt();
	vector<ModelBone *> bones;
	for (UINT i = 0; i < count; i++)
	{
		ModelBone* bone = new ModelBone();
		bone->index = r->Int();
		bone->name = String::StringToWString(r->String());
		bone->parentIndex = r->Int();
		bone->transform = r->Matrix();
		bone->absoluteTransform = r->Matrix();

		bones.push_back(bone);
	}

	count = r->UInt();
	vector<ModelMesh *> meshes;
	for (UINT i = 0; i < count; i++)
	{
		ModelMesh* mesh = new ModelMesh();
		mesh->name = String::StringToWString(r->String());
		mesh->parentBoneIndex = r->Int();

		UINT partCount = r->UInt();
		for (UINT k = 0; k < partCount; k++)
		{
			ModelMeshPart* meshPart = new ModelMeshPart();

			meshPart->parent = mesh;
			meshPart->materialName = String::StringToWString(r->String());

			//VertexData
			{
				UINT count = r->UInt();
				meshPart->vertices.assign(count, VertexTextureBlendNT());

				void* ptr = (void *)&(meshPart->vertices[0]);
				r->Byte(&ptr, sizeof(VertexTextureBlendNT) * count);
			}

			//IndexData
			{
				UINT count = r->UInt();
				meshPart->indices.assign(count, UINT());

				void* ptr = (void *)&(meshPart->indices[0]);
				r->Byte(&ptr, sizeof(UINT) * count);
			}

			//meshPart->CalcNormal();

			mesh->meshParts.push_back(meshPart);
		}//for(k)

		meshes.push_back(mesh);
	}

	r->Close();
	SafeDelete(r);


	MeshData data;
	data.Bones.assign(bones.begin(), bones.end());
	data.Meshes.assign(meshes.begin(), meshes.end());

	meshDataMap[file] = data;
}


map<wstring, vector<class ModelAnimClip *>> Models::animClipMap;
void Models::LoadAnimation(wstring file, vector<class ModelAnimClip*>* clips)
{
	if (animClipMap.count(file) < 1)
		ReadAnimation(file);

	vector<ModelAnimClip *> data = animClipMap[file];
	for (ModelAnimClip * clip : data)
	{
		ModelAnimClip* temp = NULL;
		clip->Clone((void **)&temp);

		clips->push_back(temp);
	}
}

void Models::ReadAnimation(wstring file)
{
	BinaryReader* r = new BinaryReader();
	r->Open(file);

	UINT count = r->UInt();
	vector<ModelAnimClip *> clips;
	for (UINT i = 0; i < count; i++)
	{
		ModelAnimClip* clip = new ModelAnimClip();
		clip->name = String::StringToWString(r->String());

		clip->totalFrame = r->Int();
		clip->frameRate = r->Float();
		clip->defaltFrameRate = clip->frameRate;

		UINT frameCount = r->UInt();
		for (UINT frame = 0; frame < frameCount; frame++)
		{
			ModelKeyframe* keyframe = new ModelKeyframe();
			keyframe->BoneName = String::StringToWString(r->String());

			UINT count = r->UInt();
			keyframe->Datas.assign(count, ModelKeyframeData());

			void* ptr = (void *)&(keyframe->Datas[0]);
			r->Byte(&ptr, sizeof(ModelKeyframeData) * count);

			clip->keyframeMap.insert(make_pair(keyframe->BoneName, keyframe));

		}//for(frame)


		clips.push_back(clip);
	}

	r->Close();
	SafeDelete(r);

	animClipMap[file] = clips;
}
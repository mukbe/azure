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
	//Models::LoadMaterial(file, &materials);
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




void Models::LoadMaterial(wstring file, vector<class Material*> * materials)
{
	
	Json::Value root;
	JsonHelper::ReadData(file, &root);

	Json::Value::iterator iter = root.begin();
	for (; iter != root.end(); iter++)
	{
		Json::Value value = (*iter);
		Material* material = new Material();

		string name;
		JsonHelper::GetValue(value, "Name", name);
		name = Path::GetFileNameWithoutExtension(name);
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
			string key = Path::GetFileNameWithoutExtension(textureFile);
			wstring file = String::StringToWString(textureFile);
			Texture* texture = AssetManager->AddTexture(file, key);
			material->SetDiffuseMap(texture->GetSRV());
		}
		
		JsonHelper::GetValue(value, "SpecularFile", textureFile);
		if (textureFile.length() > 0)
		{
			string key = Path::GetFileNameWithoutExtension(textureFile);
			wstring file = String::StringToWString(textureFile);
			Texture* texture = AssetManager->AddTexture(file, key);
			material->SetSpecularMap(texture->GetSRV());
		}
		
		JsonHelper::GetValue(value, "EmissiveFile", textureFile);
		if (textureFile.length() > 0)
		{
			string key = Path::GetFileNameWithoutExtension(textureFile);
			wstring file = String::StringToWString(textureFile);
			Texture* texture = AssetManager->AddTexture(file, key);
			material->SetEmissiveMap(texture->GetSRV());
		}
		
		JsonHelper::GetValue(value, "NormalFile", textureFile);
		if (textureFile.length() > 0)
		{
			string key = Path::GetFileNameWithoutExtension(textureFile);
			wstring file = String::StringToWString(textureFile);
			Texture* texture = AssetManager->AddTexture(file, key);
			material->SetNormalMap(texture->GetSRV());
		}

		materials->push_back(material);

	}


}

void Models::LoadMesh(wstring file, vector<ModelBone*>* bones, vector<ModelMesh*>* meshes)
{
	ReadMeshData(file,bones,meshes);
}

void Models::ReadMeshData(wstring file, vector<ModelBone*>* bones, vector<ModelMesh*>* meshes)
{
	BinaryReader* r = new BinaryReader();
	r->Open(file);

	UINT count = 0;

	count = r->UInt();
	//vector<ModelBone *> bones;
	for (UINT i = 0; i < count; i++)
	{
		ModelBone* bone = new ModelBone();
		bone->index = r->Int();
		bone->name = String::StringToWString(r->String());
		bone->parentIndex = r->Int();
		bone->transform = r->Matrix();
		bone->absoluteTransform = r->Matrix();

		bones->push_back(bone);
	}

	count = r->UInt();
	//vector<ModelMesh *> meshes;
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
			meshPart->materialName = Path::GetFileNameWithoutExtension(String::StringToWString(r->String()));

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

		meshes->push_back(mesh);
	}

	r->Close();
	SafeDelete(r);


}


void Models::LoadAnimation(wstring file, vector<class ModelAnimClip*>* clips)
{
	ReadAnimation(file,clips);
}

void Models::ReadAnimation(wstring file, vector<ModelAnimClip*>* clips)
{
	BinaryReader* r = new BinaryReader();
	r->Open(file);

	UINT count = r->UInt();
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


		clips->push_back(clip);
	}

	r->Close();
	SafeDelete(r);

}

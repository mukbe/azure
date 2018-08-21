#include "stdafx.h"
#include "Model.h"


#include "ModelMesh.h"
#include "ModelBone.h"
#include "ModelMeshPart.h"
#include "ModelAnimClip.h"
#include "../Utilities/BinaryFile.h"
#include "../Renders/Material.h"
#include "../Utilities/String.h"

#include <unordered_map>


Model::Model()
{
	buffer = new ModelBuffer;
}


Model::~Model()
{
	SafeDelete(buffer);

	//for (Material* material : materials)
	//	SafeDelete(material);

	for (ModelMesh* mesh : meshes)
		SafeDelete(mesh);

	for (ModelAnimClip* clip : clips)
		SafeDelete(clip);

	for (ModelBone* bone : bones)
		SafeDelete(bone);

	materials.clear();
	meshes.clear();
	clips.clear();
	bones.clear();

}

ModelMesh * Model::Mesh(wstring name)
{
	for (ModelMesh* mesh : meshes)
	{
		if (mesh->name == name)
		{
			return mesh;
		}
	}
	return nullptr;
}

ModelBone * Model::Bone(wstring name)
{
	for (ModelBone* bone : bones)
	{
		if (bone->name == name)
			return bone;
	}
	return nullptr;
}

ModelAnimClip * Model::Clip(wstring name)
{
	for (ModelAnimClip* clip : clips)
	{
		if (clip->name == name)
			return clip;
	}

	return nullptr;
}

void Model::DeleteClip(UINT index)
{
	SafeDelete(clips[index]);
	clips.erase(clips.begin() + index);
}

void Model::DeleteClip(wstring name)
{
	for (UINT i = 0; i < clips.size(); ++i)
	{
		if (clips[i]->Name() == name)
		{
			SafeDelete(clips[i]);
			clips.erase(clips.begin() + i);
			return;
		}
	}
}

void Model::CopyAbsoluteBoneTo(vector<D3DXMATRIX>& transforms)
{
	transforms.clear();
	transforms.assign(bones.size(), D3DXMATRIX());
	for (size_t i = 0; i < bones.size(); i++)
	{
		ModelBone* bone = bones[i];

		if (bone->parent != NULL)
		{
			int index = bone->parent->index;
			transforms[i] = bone->transform * transforms[index];
		}
		else
		{
			transforms[i] = bone->transform;
		}
	}

}

void Model::SaveAnimationData(wstring fileName)
{
	BinaryWriter* w = new BinaryWriter();
	w->Open(fileName);
	{
		w->UInt(this->clips.size());

		for (UINT i = 0; i < clips.size(); ++i)
		{
			ModelAnimClip* clip = clips[i];

			string clipName = String::WStringToString(clip->name);
			w->String(clipName);
			w->Int(clip->totalFrame);
			w->Float(clip->frameRate);

			UINT count = clip->keyframeMap.size();

			for (pair<wstring, ModelKeyframe*> p : clip->keyframeMap)
			{
				if (p.second == NULL)
					--count;
			}

			w->UInt(count);
			unordered_map<wstring, ModelKeyframe*>::iterator iter = clip->keyframeMap.begin();

			for (; iter != clip->keyframeMap.end(); ++iter)
			{
				if (iter->second == NULL)continue;
				string name = String::WStringToString(iter->second->BoneName);
				w->String(name);
				w->UInt(iter->second->Datas.size());
				w->Byte(&iter->second->Datas[0], sizeof(ModelKeyframeData) * iter->second->Datas.size());
			}
		}

	}
	w->Close();
	SafeDelete(w);
}

void Model::AddAnimationData(wstring fileName)
{
	BinaryReader* r = new BinaryReader;
	r->Open(fileName);
	{
		UINT count = r->UInt();

		for (UINT i = 0; i < count; ++i)
		{
			ModelAnimClip* clip = new ModelAnimClip;
			clip->name = String::StringToWString(r->String());
			clip->totalFrame = r->Int();
			clip->frameRate = r->Float();
			clip->defaltFrameRate = clip->frameRate;

			UINT size = r->UInt();

			for (UINT j = 0; j < size; ++j)
			{
				ModelKeyframe* newFrame = new ModelKeyframe();
				newFrame->BoneName = String::StringToWString(r->String());
				UINT frameCount = r->UInt();
				newFrame->Datas.assign(frameCount, ModelKeyframeData());
				void* ptr = &newFrame->Datas[0];
				r->Byte(&ptr, sizeof ModelKeyframeData * frameCount);

				clip->keyframeMap.insert(make_pair(newFrame->BoneName, newFrame));
			}

			this->clips.push_back(clip);
		}

	}
	r->Close();
	SafeDelete(r);
}
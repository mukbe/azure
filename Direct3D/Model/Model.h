#pragma once
class ModelAnimClip;
class ModelMesh;
class ModelBone;
class Material;

#define MaxBoneCount 120

class ModelBuffer : public ShaderBuffer
{
public:
	ModelBuffer() : ShaderBuffer(&data, sizeof(data))
	{
		for (int i = 0; i < MaxBoneCount; ++i)
			D3DXMatrixIdentity(&data.BoneArray[i]);
	}

	void TPose()
	{
		for (int i = 0; i < MaxBoneCount; ++i)
			D3DXMatrixIdentity(&data.BoneArray[i]);
	}

	void SetBoneTransforms(D3DXMATRIX* m, UINT count)
	{
		memcpy(data.BoneArray, m, sizeof(D3DXMATRIX) * count);
		for (UINT i = 0; i < count; ++i)
		{
			D3DXMatrixTranspose(&data.BoneArray[i], &data.BoneArray[i]);
		}
	}
private:
	struct Struct
	{
		D3DXMATRIX BoneArray[MaxBoneCount];
	}data;
};

class Model
{
	friend class ModelData;
public:
	Model();
	Model(struct ModelData data);
	~Model();

	void ReadMaterial(wstring file);
	void ReadMesh(wstring file);
	void ReadAnimation(wstring file);

	ModelBuffer* Buffer() { return buffer; }
	vector<Material*>& MaterialS() { return materials; }

	vector<ModelMesh *> const& Meshes() { return meshes; }
	vector<ModelAnimClip*> const& Clips() { return clips; }
	ModelMesh* Mesh(UINT index) { return meshes[index]; }
	ModelMesh* Mesh(wstring name);

	ModelBone* Root() { return root; }

	ModelBone* Bone(UINT index) { return bones[index]; }
	ModelBone* Bone(wstring name);
	vector<ModelBone*> Bones()const { return this->bones; }
	UINT BoneCount() { return bones.size(); }

	ModelAnimClip* Clip(UINT index) { return clips[index]; }
	ModelAnimClip* Clip(wstring name);
	void DeleteClip(UINT index);
	void DeleteClip(wstring name);
	UINT ClipCount() { return clips.size(); }
	vector<ModelAnimClip*> GetClips() const { return this->clips; }

	void CopyAbsoluteBoneTo(vector<D3DXMATRIX>& transforms);
	void SaveAnimationData(wstring fileName);
	void AddAnimationData(wstring fileName);
	void BindMeshData();

	void UIRender();
private:
	Material * GetMaterialByName(wstring name);
private:
	class ModelBone* root;

	vector<Material *> materials;
	vector<ModelMesh *> meshes;
	vector<ModelBone *> bones;
	vector<ModelAnimClip *> clips;

	ModelBuffer * buffer;
};


class Models
{
public:
	friend class Model;
public:
	static void LoadMaterial(wstring file, vector<Material *>* materials);

	static void LoadMesh(wstring file, vector<ModelBone *>* bones, vector<ModelMesh *>* meshes);
	static void ReadMeshData(wstring file,vector<ModelBone*>* bones, vector<ModelMesh*>* meshes);

	static void LoadAnimation(wstring file, vector<ModelAnimClip *>* clips);
	static void ReadAnimation(wstring file,vector<ModelAnimClip*>* clips);

};
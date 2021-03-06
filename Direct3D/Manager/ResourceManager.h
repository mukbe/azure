#pragma once

struct MeshDatas
{
	vector<class ModelBone *> Bones;
	vector<class ModelMesh *> Meshes;
};

struct ModelData
{
	wstring file;
	vector<class Material*>			materials;
	MeshDatas						meshDatas;
	vector<class ModelAnimClip*>	animations;
	vector<class GameCollider*>		colliders;

	void Clone(vector<class Material*>* pMaterials, vector<class ModelBone*>* pBones, 
		vector<class ModelMesh*>* pMeshes, vector<class ModelAnimClip*>* pAnim,vector<class GameCollider*>* colliders = nullptr);
	void Clone(class Model* model);

	void Release();
};

class ResourceManager
{
public:
	SingletonHeader(ResourceManager)
private:
	typedef map<string, class Texture*> TextureContainer;
	typedef map<string, class Texture*>::iterator TextureIter;
	typedef map<string, ModelData> ModelContainer;
	typedef map<string, ModelData>::iterator ModelContainerIter;
public:
	void UIRender();
	void Open() { this->isShow = true; }
	void Close() { this->isShow = false; }
	void LoadAsset();
	void LoadFolder(const std::string& path, const std::string& filter,bool isAnim = false);

	class Texture* AddTexture(wstring file, string keyName);
	class Texture* AddTexture(string key, class Texture* texture);
	class Texture* FindTexture(string keyName);
	ID3D11ShaderResourceView* GetRsourceView(string keyName);

	ModelData AddModelData(wstring file, string keyName, bool isAnimation = false);
	ModelData AddModelData(wstring file, bool isAnimation = false);
	ModelData GetModelData(string keyName);
	class Model* GetModel(wstring file, string keyName, bool isAnimation = false);
	class Model* GetModel(wstring file, bool isAnimation = false);
private:
	bool isShow;
	TextureContainer textures;
	ModelContainer models;
};

#define AssetManager ResourceManager::Get()

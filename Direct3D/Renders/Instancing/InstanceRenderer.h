#pragma once

struct InstanceData
{
	D3DXVECTOR4 data[3];
};

class InstanceRenderer
{
private:
	string name;
	UINT maxInstanceCount;
	UINT drawInstanceCout;
	class InstanceShader* shader;
	ID3D11Buffer* instanceBuffer;
private:
	class ModelBone*					root;
	vector<class Material *>			materials;
	vector<class ModelMesh *>			meshes;
	vector<class ModelBone *>			bones;
private:
	vector<class GameObject*>			instanceList;
	vector<D3DXMATRIX>					localTransforms;
public:
	void CopyAbsoluteBoneTo();
	Material * GetMaterialByName(wstring name);
public:
	InstanceRenderer(string name,UINT maxInstance);
	~InstanceRenderer();

	void ReadMaterial(wstring file);
	void ReadMesh(wstring file);
	void CreateBuffer();

	void UpdateBuffer();
	void Render();

	void AddInstanceData(class GameObject* mat);
};


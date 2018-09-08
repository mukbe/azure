#pragma once

struct InstanceData
{
	D3DXVECTOR4 data[3];
};

class InstanceRenderer
{
private:
	Synthesize(string,name,Name)
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
private:
	Material * GetMaterialByName(wstring name);
	void ReadMaterial(wstring file);
	void ReadMesh(wstring file);
	void CopyAbsoluteBoneTo();
	void CreateBuffer();
public:
	InstanceRenderer(string name,UINT maxInstance);		//툴작업용 생성자
	InstanceRenderer(string name,wstring fileName);		//플레이씬용 생정자 
	~InstanceRenderer();

	void InitData(wstring materiakFile, wstring meshFile);
	void UpdateBuffer();
	void Render();

	void AddInstanceData(class GameObject* pOject);
};


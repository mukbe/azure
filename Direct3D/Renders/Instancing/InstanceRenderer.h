#pragma once
#include "./Object/GameObject/GameObject.h"
struct InstanceData
{
	D3DXVECTOR4 data[3];
};

class InstanceRenderer : public GameObject
{
private:
	UINT maxInstanceCount;
	UINT drawInstanceCount;
	class InstanceShader* shader;
	ID3D11Buffer* instanceBuffer;
private:
	class ModelBone*					root;
	vector<class Material *>			materials;
	vector<class ModelMesh *>			meshes;
	vector<class ModelBone *>			bones;
	vector<class GameCollider*>			colliders;
private:
	vector<class GameObject*>			instanceList;
	vector<D3DXMATRIX>					localTransforms;
private:
	class Material * GetMaterialByName(wstring name);
	void BindMeshData();
	void CreateBuffer();
	void UpdateBuffer();
	void AddInstance(float autoSize);

	
public:
	InstanceRenderer(string name,UINT maxInstance);		//툴작업용 생성자
	InstanceRenderer(string name,wstring fileName);		//플레이씬용 생정자 
	~InstanceRenderer();

	void InitializeData(string keyName);

	virtual void Release()override;
	virtual void PostUpdate()override;
	virtual void Render()override;
	virtual void UIRender()override;
	virtual void UIUpdate()override;

	void AddInstanceData(class GameObject* pOject);
};


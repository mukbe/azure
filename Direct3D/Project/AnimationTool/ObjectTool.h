#pragma once

class ObjectTool
{
private:
	class DebugTransform* debugTransform;
	class Model* model;
	class Shader* shader;
	class GameCollider* targetCollider;
	vector<class GameCollider*> colliderList;
	bool zBufferOff;
public:
	ObjectTool();
	~ObjectTool();

	void Update();
	void Render();
	void UIRender();

	void SetCamera(class FreeCamera* camera);
private:
	void GetModelData(struct ModelData* pData);
	void LoadMaterial(wstring file = L"");
	void LoadMesh(wstring file = L"");

	void ChoosePart();
	void CreateColliderAuto(class ModelMeshPart* part);
	void AddCollider();
	void DeleteCollider();
	void SaveCollider(wstring file = L"");
	void LoadCollider(wstring file = L"");
	void ReleaseCollider();

	void ChangeTargetCollider(class GameCollider* collider);
};


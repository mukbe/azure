#pragma once
#include "./Project/AnimationTool/FBX/Exporter.h"
#include <thread>

namespace Environment
{
	class Sun;
}
class AnimationTool :public SceneNode
{
private:
	class FreeCamera* freeCamera;
	class Figure* box, *grid, *sphere;
	class DirectionalLight* directionalLight;
	class CharacterTool* characterTool;
public:
	AnimationTool();
	virtual ~AnimationTool();
	
	void Init();
	void Release();
	void PreUpdate();
	void Update();
	void PostUpdate();
	void ShadowRender();
	void Render();
	void UIRender();
private:

	void AttachModel(class Model* model);
	void LoadModel();
	void LoadMaterial(wstring fileName = L"");
	void LoadMesh(wstring fileName = L"");
	void LoadAnimation(wstring fileName = L"");
	void ExportModel(wstring fileName = L"");
	void ExportMaterial(wstring fileName = L"");
	void ExportMesh(wstring fileName = L"");
	void ExportAnimation(wstring fileName = L"");

	void AddAnimation(wstring fileName = L"");
	void ReNameAnimation(class ModelAnimClip* clip);
	void DeleteAnimation(wstring name);

	void SaveAnimation(wstring fileName = L"");
	void SaveCollider(wstring fileName = L"");
	void LoadCollider(wstring fileName = L"");

	void ShowBoneData();

	void AddCollider();
	void ReleaseCollider();
	//void CameraInfoRender();
private:
	class FreeCamera* freeCamera;
	class Figure* box, *grid, *sphere;
	Environment::Sun* sun;


	thread* load;
	thread* loadMesh;
	thread* loadAni;

	bool bLoadedMat;
	bool bLoadedMesh;
	bool bLoadedAni;

};


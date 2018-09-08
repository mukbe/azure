#pragma once
class CharacterTool
{
private:
	class ModelAnimPlayer* animation;
	class Model* model;
	vector<class AnimationCollider*> colliderList;
	class AnimationCollider* targetCollider;
	class DebugTransform* debugTransform;

	class FreeCamera* freeCamera;
	class Model* tempItemModel;
private:	//For Gui
	bool showDemo;
	bool showTool;
	bool showBone;
	bool debugControl;
	int selectBoneIndex;
public:
	CharacterTool();
	~CharacterTool();

	void Init();
	void Release();
	void Update();
	void Render();
	void UIRender();


	void SetCamera(class FreeCamera* camera);
private:
	void AttachModel(class Model* model);

	void AddAnimation(wstring fileName = L"");
	void ReNameAnimation();
	void DeleteAnimation();

	void SaveAnimation(wstring fileName = L"");
	void SaveCollider(wstring fileName = L"");
	void LoadCollider(wstring fileName = L"");

	void ShowBoneData();

	void AddCollider();
	void ReleaseCollider();
	void DeleteCollider();

	void ChangeTargetCollider(class AnimationCollider* collider,bool isFirst);
	void ControlCollider();

	void FindPathMaterial(wstring fileName = L"");
	void FindPathMesh(wstring fileName = L"");
private:
	void RenderAnimationTool();
	void RenderBoneTool();
	void RenderColliderTool();
};


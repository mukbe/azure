#pragma once
#include "./Project/AnimationTool/FBX/Exporter.h"
class AnimationTool
{
private:
	class ModelAnimPlayer* animation;
	class Model* model;
	class Fbx::Exporter *exporter;
	Synthesize(bool, isRenderUI, IsRenderUI)
	bool shdowDemo;
	bool isPlay;
public:
	AnimationTool();
	~AnimationTool();
	
	void Init();
	void Release();
	void Update();
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
};


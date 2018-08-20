#include "stdafx.h"
#include "AnimationTool.h"

#include "./Model/ModelAnimPlayer.h"
#include "./Model/Model.h"
#include "./Model/ModelBone.h"
#include "./Model/ModelMesh.h"
#include "./Model/ModelAnimClip.h"

#include "./Project/AnimationTool/FBX/Exporter.h"

#include "./Utilities/String.h"
#include "./Utilities/Path.h"

AnimationTool::AnimationTool()
	:animation(nullptr), model(nullptr), isRenderUI(false), isPlay(false), exporter(nullptr), shdowDemo(nullptr)
{
	animation = new ModelAnimPlayer(nullptr);
}


AnimationTool::~AnimationTool()
{
	SafeDelete(exporter);
	SafeDelete(animation);
	SafeDelete(model);
}

void AnimationTool::Init()
{
}

void AnimationTool::Release()
{
}

void AnimationTool::Update()
{
	animation->Update();
}

void AnimationTool::Render()
{
	animation->Render();
}

void AnimationTool::UIRender()
{
	//MainBar
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("AnimationFile"))
		{
			if (ImGui::MenuItem("Tool"))isRenderUI = !isRenderUI;
			if (ImGui::MenuItem("LoadModel"))this->LoadModel();
			if (ImGui::MenuItem("ExportModel"))this->ExportModel();
			if (ImGui::MenuItem("Demo"))shdowDemo = !shdowDemo;
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (isRenderUI)
	{
		if (this->model == nullptr || this->animation == nullptr)return;
		ImGui::Begin("AnimationTool");
		{
			ImGui::Text("FrameTime : %f", animation->GetFrameTime());
			ImGui::Separator();

			if (ImGui::Checkbox("Play", &isPlay))
			{
				if (isPlay)this->animation->Play();
				else this->animation->Pause();
			}
		}
		ImGui::End();
	}

	if (shdowDemo)
		ImGui::ShowDemoWindow();
}

void AnimationTool::AttachModel(Model * model)
{
	if (animation == nullptr)
		animation = new ModelAnimPlayer(model);
	else
	{
		animation->Init();
		animation->SetAnimationModel(model);
	}
}

void AnimationTool::LoadModel()
{
	SafeDelete(model);
	model = new Model;
	this->animation->Init();
	
	this->LoadMaterial();
	this->LoadMesh();
	this->LoadAnimation();

	this->animation->SetAnimationModel(model);
	this->animation->ChangeAnimation(0);
	this->isPlay = true;
}

void AnimationTool::LoadMaterial(wstring fileName)
{
	if (fileName.length() > 0)
	{
		model->ReadMaterial(fileName);
	}
	else
	{
		function<void(wstring)> func = std::bind(&AnimationTool::LoadMaterial, this, placeholders::_1);
		Path::OpenFileDialog(L"", Path::MaterialFilter, Assets, func);
	}
}

void AnimationTool::LoadMesh(wstring fileName)
{
	if (fileName.length() > 0)
	{
		model->ReadMesh(fileName);
	}
	else
	{
		function<void(wstring)> func = std::bind(&AnimationTool::LoadMesh, this, placeholders::_1);
		Path::OpenFileDialog(L"", Path::MeshFilter, Assets, func);
	}
}

void AnimationTool::LoadAnimation(wstring fileName)
{
	if (fileName.length() > 0)
	{
		model->ReadAnimation(fileName);
	}
	else
	{
		function<void(wstring)> func = std::bind(&AnimationTool::LoadAnimation, this, placeholders::_1);
		Path::OpenFileDialog(L"", Path::AnimationFilter, Assets, func);
	}
}

void AnimationTool::ExportModel(wstring fileName)
{
	if (fileName.length() > 0)
	{
		exporter = new Fbx::Exporter(fileName);
		this->ExportMaterial();
		this->ExportMesh();
		this->ExportAnimation();
		SafeDelete(exporter);
	}
	else
	{
		function<void(wstring)> func = std::bind(&AnimationTool::ExportModel, this, placeholders::_1);
		Path::OpenFileDialog(L"", nullptr, Assets, func);
	}
}

void AnimationTool::ExportMaterial(wstring fileName)
{
	if (fileName.length() > 0)
	{
		if (exporter)
			exporter->ExportMaterial(Assets,fileName);
	}
	else
	{
		function<void(wstring)> func = std::bind(&AnimationTool::ExportMaterial, this, placeholders::_1);
		Path::SaveFileDialog(fileName, Path::MaterialFilter, Assets, func);
	}
}

void AnimationTool::ExportMesh(wstring fileName)
{
	if (fileName.length() > 0)
	{	
		if(exporter)
			exporter->ExportMesh(Assets, fileName);
	}
	else
	{
		function<void(wstring)> func = std::bind(&AnimationTool::ExportMesh, this, placeholders::_1);
		Path::SaveFileDialog(fileName, Path::MeshFilter, Assets, func);
	}
}

void AnimationTool::ExportAnimation(wstring fileName)
{
	if (fileName.length() > 0)
	{
		if (exporter)
			exporter->ExportAnimation(Assets, fileName);
	}
	else
	{
		function<void(wstring)> func = std::bind(&AnimationTool::ExportAnimation, this, placeholders::_1);
		Path::SaveFileDialog(fileName, Path::AnimationFilter, Assets, func);
	}
}

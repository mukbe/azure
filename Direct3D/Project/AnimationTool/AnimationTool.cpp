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


#include "./View/FreeCamera.h"
#include "./Utilities/Transform.h"
#include "./Figure/Figure.h"

#include "./Testing/DirectionalLight.h"

#include "./Renders/DeferredRenderer.h"


AnimationTool::AnimationTool()
	:animation(nullptr), model(nullptr), isRenderUI(false), isPlay(false), exporter(nullptr), shdowDemo(nullptr), selectClipIndex(0), selectedIndex(0)
	, isLoadModel(false)
{
	RenderRequest->AddRender("UIRender", bind(&AnimationTool::UIRender, this), RenderType::UIRender);
	RenderRequest->AddRender("shadow", bind(&AnimationTool::ShadowRender, this), RenderType::Shadow);
	RenderRequest->AddRender("render", bind(&AnimationTool::Render, this), RenderType::Render);

	animation = new ModelAnimPlayer(nullptr);

	//model = new Model;
	//model->ReadMaterial(L"../_Assets/wow.material");
	//model->ReadMesh(L"../_Assets/wow.mesh");
	//model->ReadAnimation(L"../_Assets/wow.anim");
	//this->AttachModel(model);

	//Fbx::Exporter* exporter = new Fbx::Exporter(L"../_Assets/wow.fbx");
	//exporter->ExportMaterial(Assets, L"wow");
	//exporter->ExportMesh(Assets, L"wow");
	//exporter->ExportAnimation(Assets, L"wow");
	//SafeDelete(exporter);

	freeCamera = new FreeCamera();

	grid = new Figure(Figure::FigureType::Grid, 100.0f, D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f));

	directionalLight = new DirectionalLight;
}


AnimationTool::~AnimationTool()
{
	SafeDelete(exporter);
	SafeDelete(animation);
	SafeDelete(model);

	SafeDelete(grid);

	SafeDelete(directionalLight);
	SafeDelete(freeCamera);

}

void AnimationTool::Init()
{
}

void AnimationTool::Release()
{
}

void AnimationTool::PreUpdate()
{
}

void AnimationTool::Update()
{
	if (KeyCode->Down(VK_F1))
		animation->ChangeAnimation(0);
	if (KeyCode->Down(VK_F2))
		animation->ChangeAnimation(1);

	animation->Update();
}

void AnimationTool::PostUpdate()
{
	freeCamera->Update();
}

void AnimationTool::ShadowRender()
{
	directionalLight->UpdateView();
	directionalLight->SetBuffer();

	freeCamera->Render();
	States::SetRasterizer(States::SHADOW);
	grid->ShadowRender();
	States::SetRasterizer(States::SOLID_CULL_ON);

}

void AnimationTool::Render()
{
	freeCamera->Render();
	grid->Render();
	animation->Render();

	//camera정보를 deferred에게 언팩킹시에 필요한 정보를 보낸다
	DeferredRenderer*deferred = (DeferredRenderer*)RenderRequest->GetDeferred();
	deferred->SetUnPackInfo(freeCamera->GetViewMatrix(), freeCamera->GetProjection());
	directionalLight->SetBuffer();

	States::SetSampler(1, States::LINEAR);

}

void AnimationTool::UIRender()
{

	ImGui::Begin("System Info");
	ImGui::Text("Frame Per Second : %4.0f", ImGui::GetIO().Framerate);
	ImGui::Text("TimeDelta : %f", DeltaTime);

	UINT hour = Time::Get()->GetHour();
	string hourStr = hour < 10 ? "0" + to_string(hour) : to_string(hour);

	UINT minute = Time::Get()->GetMinute();
	string minuteStr = minute < 10 ? "0" + to_string(minute) : to_string(minute);

	ImGui::Text("%s", (hourStr + ":" + minuteStr).c_str());

	ImGui::Separator();

	D3DXVECTOR3 pos = freeCamera->GetTransform()->GetWorldPosition();
	ImGui::Text
	(
		"Camera Position : %3.0f, %3.0f, %3.0f"
		, pos.x, pos.y, pos.z
	);

	D3DXVECTOR3 angle = freeCamera->GetTransform()->GetAngle();
	ImGui::Text
	(
		"Camera Rotation : %3.0f, %3.0f", angle.x * 180.f / D3DX_PI, angle.y *180.f / D3DX_PI
	);

	ImGui::Separator();

	ImGui::End();

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
			if (ImGui::Button("Add", ImVec2(80, 50)))
				this->AddAnimation();
			ImGui::SameLine();
			if (ImGui::Button("Delete", ImVec2(80, 50)))
				this->DeleteAnimation(String::StringToWString(selectedAnimation));
			ImGui::SameLine();
			if (ImGui::Button("ReName", ImVec2(80, 50)))
				ImGui::OpenPopup("ReNameAnimation");
			ImGui::SameLine();
			if (ImGui::Button("Save",ImVec2(80,50)))
				this->SaveAnimation();

			ImGui::Separator();
			ImGui::Text("FrameTime : %f", animation->GetFrameTime());
			ImGui::Separator();

			if (ImGui::Checkbox("Play", &isPlay))
			{
				if (isPlay)this->animation->Play();
				else this->animation->Pause();
			}

			//AnimationList
			vector<ModelAnimClip*> clips = this->animation->GetModel()->GetClips();
			if(ImGui::BeginCombo("AnimationList", comboStr.c_str()))
			{
				for (UINT i = 0; i < clips.size(); ++i)
				{
					string clipName = String::WStringToString(clips[i]->Name());
					bool isSelected = false;
					if (ImGui::Selectable(clipName.c_str(), isSelected))
					{
						comboStr = clipName;
						selectedIndex = (int)i;
					}
					if (isSelected);
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		
			if (ImGui::BeginPopupModal("ReNameAnimation", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				this->ReNameAnimation(model->GetClips()[selectedIndex]);
				ImGui::EndPopup();
			}

		}
		ImGui::End();
	}

	if (shdowDemo)
		ImGui::ShowDemoWindow();

	if (selectedAnimation != comboStr)
	{
		animation->ChangeAnimation(String::StringToWString(comboStr));
		selectedAnimation = comboStr;
	}

	
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
	this->animation->Init();
	SafeDelete(model);
	model = new Model;
	
	this->LoadMaterial();
	this->LoadMesh();
	this->LoadAnimation();

	this->animation->SetAnimationModel(model);
	this->animation->ChangeAnimation(0);
	this->isPlay = true;
	this->comboStr = String::WStringToString(model->Clip(0)->Name()).c_str();
	this->selectedIndex = 0;
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
			exporter->ExportMaterial(L"",fileName);
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
			exporter->ExportMesh(L"", fileName);
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
			exporter->ExportAnimation(L"", fileName);
	}
	else
	{
		function<void(wstring)> func = std::bind(&AnimationTool::ExportAnimation, this, placeholders::_1);
		Path::SaveFileDialog(fileName, Path::AnimationFilter, Assets, func);
	}
}

void AnimationTool::AddAnimation(wstring fileName)
{
	if (this->model == nullptr)return;
	
	if (fileName.length() > 0)
	{
		model->ReadAnimation(fileName);
	}
	else
	{
		function<void(wstring)> func = std::bind(&AnimationTool::AddAnimation, this, placeholders::_1);
		Path::OpenFileDialog(L"", Path::AnimationFilter, Assets, func);
	}

}

void AnimationTool::ReNameAnimation(ModelAnimClip * clip)
{
	static char str[48] = {};
	ImGui::InputText("Name", str, sizeof(char) * 48);
	if (ImGui::Button("Accept"))
	{
		wstring newName = String::StringToWString(str);
		clip->SetName(newName);
		this->selectedAnimation = this->comboStr = str;

		ImGui::CloseCurrentPopup();
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancle"))
		ImGui::CloseCurrentPopup();
}

void AnimationTool::DeleteAnimation(wstring name)
{
	model->DeleteClip(name);
	animation->ChangeAnimation(0);
	this->selectedIndex = 0;
	this->comboStr = this->selectedAnimation = String::WStringToString(animation->GetCurrentClip()->Name());
}

void AnimationTool::SaveAnimation(wstring fileName)
{
	if (fileName.length() > 0)
	{
		model->SaveAnimationData(fileName);
	}
	else
	{
		function<void(wstring)> func = std::bind(&AnimationTool::SaveAnimation, this, placeholders::_1);
		Path::SaveFileDialog(fileName, Path::AnimationFilter, Assets, func);
	}
}


#include "stdafx.h"
#include "Program.h"

#include "./View/FreeCamera.h"
#include "./Utilities/Transform.h"
#include "./Figure/Figure.h"
#include "./Renders/DeferredRenderer.h"

#include "./Renders/ShadowRenderer.h"
#include "./Testing/DirectionalLight.h"

#include "./Project/AnimationTool/FBX/Exporter.h"
#include "./Model/Model.h"
#include "Model/ModelAnimPlayer.h"

#include "./Utilities/DebugTransform.h"

Program::Program()
{
	States::Create();
	
	D3DDesc desc;
	DxRenderer::GetDesc(&desc);

	jsonRoot = new Json::Value();
	JsonHelper::ReadData(L"LevelEditor.json", jsonRoot);

	//Fbx::Exporter* exporter = new Fbx::Exporter(L"../_Assets/Test.fbx");
	//exporter->ExportMesh(L"../_Assets/", L"Test");
	//exporter->ExportMaterial(L"../_Assets/", L"Test");
	//exporter->ExportAnimation(L"../_Assets/", L"Test");

	//Model* model = new Model;
	//model->ReadMaterial(L"../_Assets/Test.material");
	//model->ReadMesh(L"../_Assets/Test.mesh");

	//model->ReadAnimation(L"../_Assets/Test.anim");
	//this->animation = new ModelAnimPlayer(model);

	freeCamera = new FreeCamera();
	
	box = new Figure(Figure::FigureType::Box, 1.f);
	box->GetTransform()->SetWorldPosition(50.0f, 5.0f, 50.0f);
	box->GetTransform()->RotateSelf(0.f, 45.0f * ONE_RAD, 0.f);
	box->GetTransform()->SetScale(10.f, 10.f, 10.f);
	grid = new Figure(Figure::FigureType::Grid, 100.0f,D3DXCOLOR(0.3f,0.3f,0.3f,1.0f));

	sphere = new Figure(Figure::FigureType::Sphere, 10.0f, D3DXCOLOR(1.f, 0.f, 0.f, 1.f));

	deferred = new DeferredRenderer;
	directionalLight = new DirectionalLight;
	shadow = new ShadowRenderer;

	shadow->SetRenderFunc(bind(&Program::ShadowRender,this));

	debugTransform = new DebugTransform;
	debugTransform->ConnectTransform(box->GetTransform());
	debugTransform->SetCamera(freeCamera);
}

Program::~Program()
{
	SafeDelete(deferred);
	SafeDelete(sphere);
	SafeDelete(grid);
	SafeDelete(box);

	JsonHelper::WriteData(L"LevelEditor.json", jsonRoot);
	SafeDelete(jsonRoot);

	States::Delete();
}

void Program::PreUpdate()
{
}

void Program::Update()
{
	//animation->Update();
	debugTransform->Update();
}

void Program::PostUpdate()
{
	freeCamera->Update();
}

void Program::ShadowRender()
{
	freeCamera->Render();
	States::SetRasterizer(States::SHADOW);
	grid->ShadowRender();
	box->ShadowRender();
	sphere->ShadowRender();
	States::SetRasterizer(States::SOLID_CULL_ON);
}

void Program::PreRender()
{
	directionalLight->UpdateView();
	directionalLight->SetBuffer();

	//Program::ShodowRender
	shadow->RenderDirectionalMap();
}

void Program::Render()
{	
	deferred->BegindDrawToGBuffer();
	freeCamera->Render();
	grid->Render();
	box->Render();
	sphere->Render();
	//animation->Render();
	debugTransform->RenderGizmo();
	
	//camera정보를 deferred에게 언팩킹시에 필요한 정보를 보낸다
	deferred->SetUnPackInfo(freeCamera->GetViewMatrix(), freeCamera->GetProjection());
}

void Program::PostRender()
{
	//bind ShadowMap
	ID3D11ShaderResourceView* view = shadow->GetDirectionalSRV();
	DeviceContext->PSSetShaderResources(5, 1, &view);
	shadow->BindResources();

	directionalLight->SetBuffer();

	States::SetSampler(1, States::LINEAR);
	deferred->Render();
}

void Program::UIRender()
{
	deferred->UIRender();
	debugTransform->RenderGUI();

	ImGui::Begin("ShadowMap");
	{
		ImGui::ImageButton(shadow->GetDirectionalSRV(), ImVec2(200, 200));
	}
	ImGui::End();

	ImGui::Begin("System Info");
	ImGui::Text("Frame Per Second : %4.0f", ImGui::GetIO().Framerate);

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

}

void Program::ResizeScreen()
{
	D3DDesc desc;
	DxRenderer::GetDesc(&desc);

}


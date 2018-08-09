#include "stdafx.h"
#include "Program.h"

#include "./View/FreeCamera.h"
#include "./Utilities/Transform.h"
#include "./Figure/Figure.h"
#include "./Renders/DeferredRenderer.h"

Program::Program()
{
	States::Create();

	D3DDesc desc;
	DxRenderer::GetDesc(&desc);

	jsonRoot = new Json::Value();
	Json::ReadData(L"LevelEditor.json", jsonRoot);

	freeCamera = new FreeCamera();
	
	box = new Figure(Figure::FigureType::Box, 10.0f);
	box->GetTransform()->SetWorldPosition(50.0f, 5.0f, 50.0f);
	box->GetTransform()->RotateSelf(0.f, 45.0f * ONE_RAD, 0.f);
	grid = new Figure(Figure::FigureType::Grid, 100.0f,D3DXCOLOR(0.3f,0.3f,0.3f,1.0f));
	sphere = new Figure(Figure::FigureType::Sphere, 10.0f, D3DXCOLOR(1.f, 0.f, 0.f, 1.f));

	deferred = new DeferredRenderer;
}

Program::~Program()
{
	SafeDelete(deferred);
	SafeDelete(sphere);
	SafeDelete(grid);
	SafeDelete(box);

	Json::WriteDate(L"LevelEditor.json", jsonRoot);
	SafeDelete(jsonRoot);

	States::Delete();
}

void Program::PreUpdate()
{
}

void Program::Update()
{

}

void Program::PostUpdate()
{
	freeCamera->Update();
}

void Program::PreRender()
{
	freeCamera->Render();
}

void Program::Render()
{
	deferred->BegindDrawToGBuffer();

	grid->Render();
	box->Render();
	sphere->Render();

	pRenderer->EndShadowDraw();
	pRenderer->BeginDraw();
	deferred->Render();
}

void Program::PostRender()
{
	deferred->PostRender();


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


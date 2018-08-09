#include "stdafx.h"
#include "Program.h"

#include "./View/FreeCamera.h"

Program::Program()
{
	States::Create();

	D3DDesc desc;
	D3D::GetDesc(&desc);


	//values->JsonRoot = new Json::Value();
	//Json::ReadData(L"LevelEditor.json", values->JsonRoot);
	freeCamera = new FreeCamera();

}

Program::~Program()
{

	//Json::WriteDate(L"LevelEditor.json", values->JsonRoot);
	//SafeDelete(values->JsonRoot);


	States::Delete();
}

void Program::Update()
{
	freeCamera->Update();


}

void Program::PreRender()
{
}

void Program::Render()
{
	freeCamera->Render();

	GizmoRenderer->WireSphere(D3DXVECTOR3(0, 0, 0), 3.f, D3DXCOLOR(1, 1, 0, 1));


}

void Program::PostRender()
{

}

void Program::ResizeScreen()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

}


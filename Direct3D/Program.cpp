#include "stdafx.h"
#include "Program.h"

Program::Program()
{
	States::Create();

	D3DDesc desc;
	D3D::GetDesc(&desc);


	//values->JsonRoot = new Json::Value();
	//Json::ReadData(L"LevelEditor.json", values->JsonRoot);



}

Program::~Program()
{

	//Json::WriteDate(L"LevelEditor.json", values->JsonRoot);
	//SAFE_DELETE(values->JsonRoot);


	States::Delete();
}

void Program::Update()
{


}

void Program::PreRender()
{
}

void Program::Render()
{


}

void Program::PostRender()
{

}

void Program::ResizeScreen()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

}


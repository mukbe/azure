#include "stdafx.h"
#include "ParticleTool.h"

ParticleTool::ParticleTool()
{
}


ParticleTool::~ParticleTool()
{
}

void ParticleTool::Update()
{
}

void ParticleTool::Render()
{
}

void ParticleTool::UIRender()
{
}

void ParticleTool::LoadData(wstring file)
{
	wstring str = Assets + L"ParticleData/" + file + L".json";
	JsonHelper::ReadData(str, jsonRoot);
	Json::Value prop = (*jsonRoot)["EmitData"];

	prop = (*jsonRoot)["EmitData"];





}

void ParticleTool::SaveData(wstring file)
{
}

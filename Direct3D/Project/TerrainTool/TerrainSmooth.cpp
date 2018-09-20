#include "stdafx.h"
#include "TerrainSmooth.h"



TerrainSmooth::TerrainSmooth()
{
	computeSmooth = new ComputeShader(ShaderPath + L"TerrainToolCS.hlsl", "Smooth");
	computeALL = new ComputeShader(ShaderPath + L"TerrainToolCS.hlsl", "CopyALLHeightMap");

}


TerrainSmooth::~TerrainSmooth()
{
	SafeDelete(computeSmooth);
	SafeDelete(computeALL);

}

void TerrainSmooth::CopyHeight()
{
	computeALL->BindShader();
	computeALL->Dispatch(16, 16, 1);
}

void TerrainSmooth::EditHeight()
{
	computeSmooth->BindShader();
	computeSmooth->Dispatch(16, 16, 1);

}

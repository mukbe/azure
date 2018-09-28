#include "stdafx.h"
#include "TerrainHeight.h"


TerrainHeight::TerrainHeight()
	: heightSet(nullptr), heightMap(nullptr)
{
	heightSet = new ComputeShader(ShaderPath + L"Copy.hlsl", "CopyTexture");
	heightMap = new CResource2D(256, 256, DXGI_FORMAT_R8G8B8A8_UNORM);


	texture = new Texture(Contents + L"HeightMap256.png");

	computeHeightEdit = new ComputeShader(ShaderPath + L"TerrainToolCS.hlsl", "HeightEdit");
	computeCopyHeight = new ComputeShader(ShaderPath + L"TerrainToolCS.hlsl", "CopyHeightMap");
	tempHeightMap = new CResource1D(sizeof(D3DXVECTOR4), 256 * 256, nullptr);

	heightSet->BindShader();
	heightMap->BindResource(0);
	texture->SetCSResource(0);

	heightSet->Dispatch(16, 16, 1);

	ID3D11ShaderResourceView* nullsrv[1] = { nullptr };
	DeviceContext->CSSetShaderResources(0, 1, nullsrv);
	heightMap->ReleaseResource(0);

}


TerrainHeight::~TerrainHeight()
{
	SafeDelete(tempHeightMap);
	SafeDelete(computeCopyHeight);
	SafeDelete(computeHeightEdit);

}

void TerrainHeight::CopyHeight()
{
	computeCopyHeight->BindShader();
	tempHeightMap->BindResource(2);

	computeCopyHeight->Dispatch(16, 16, 1);

	tempHeightMap->ReleaseResource(2);
}

void TerrainHeight::EditHeight()
{
	computeHeightEdit->BindShader();
	tempHeightMap->BindResource(2);

	computeHeightEdit->Dispatch(16, 16, 1);
	tempHeightMap->ReleaseResource(2);

}

void TerrainHeight::Load()
{
	heightSet->BindShader();
	heightMap->BindResource(0);

	heightSet->Dispatch(16, 16, 1);
}

ID3D11ShaderResourceView * TerrainHeight::GetHeightMap()
{
	return heightMap->GetSRV();
}

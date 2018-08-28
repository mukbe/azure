#include "stdafx.h"
#include "TerrainHeight.h"
#include "./Renders/Texture.h"


TerrainHeight::TerrainHeight()
	: heightSet(nullptr), heightMap(nullptr)
{
	heightSet = new ComputeShader(ShaderPath + L"Copy.hlsl", "CopyTexture");
	heightMap = new CResource2D(256, 256);

	texture = new Texture(Contents + L"HeightMap256.png");

	heightSet->BindShader();
	heightMap->BindResource(0);
	texture->SetCSResource(0);

	heightSet->Dispatch(17, 17, 1);

	ID3D11ShaderResourceView* nullsrv[1] = { nullptr };
	DeviceContext->CSSetShaderResources(0, 1, nullsrv);
	heightMap->ReleaseResource(0);

}


TerrainHeight::~TerrainHeight()
{
}

ID3D11ShaderResourceView * TerrainHeight::GetHeightMap()
{
	return heightMap->GetSRV();
}

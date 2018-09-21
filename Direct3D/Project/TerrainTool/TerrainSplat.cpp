#include"stdafx.h"
#include "TerrainSplat.h"
#include "./Utilities/ImGuiHelper.h"


TerrainSplat::TerrainSplat()
{
	computeAlphaSplat = new ComputeShader(ShaderPath + L"TerrainToolCS.hlsl", "Splat");
	computeCopyTex = new ComputeShader(ShaderPath + L"Copy.hlsl", "CopyTexture");

	splatMap = new CResource2D(256, 256);
	tempMap = new CResource2D(256, 256);


	texture.assign(4, nullptr);
	//texture[0] = new Texture(Contents + L"Grass1.png");

	buffer = new Buffer;
}


TerrainSplat::~TerrainSplat()
{
	SafeDelete(splatMap);
	SafeDelete(computeAlphaSplat);
}

void TerrainSplat::Splat()
{
	//Copy
	computeCopyTex->BindShader();
	tempMap->BindResource(0);
	splatMap->BindCSShaderResourceView(0);
	computeCopyTex->Dispatch(16, 16, 1);
	splatMap->ReleaseCSshaderResorceView(0);
	tempMap->ReleaseResource(0);

	//Update
	computeAlphaSplat->BindShader();

	buffer->SetCSBuffer(2);

	splatMap->BindResource(3);
	tempMap->BindCSShaderResourceView(3);

	computeAlphaSplat->Dispatch(16, 16, 1);

	tempMap->ReleaseCSshaderResorceView(3);
	splatMap->ReleaseResource(3);


}

void TerrainSplat::Render()
{
	splatMap->BindPSShaderResourceView(5);

	ID3D11ShaderResourceView* srv[4] = { nullptr,nullptr ,nullptr ,nullptr };
	for (int i = 0; i < 4; i++)
	{
		if (texture[i] != nullptr)
		{
			srv[i] = texture[i]->GetSRV();
		}
	}

	DeviceContext->PSSetShaderResources(6, 4, srv);
}

void TerrainSplat::TerrainUI()
{
	if (ImGui::InputFloat("SplatAmount", &buffer->Data.SplatAmount, 0.0001f, 0.1f))
	{
		buffer->Data.SplatAmount = Math::Clamp(buffer->Data.SplatAmount, -1.0f, 1.0f);
	}
	if (ImGui::InputInt("SplatNum", &buffer->Data.SplatNum, 1))
	{
		buffer->Data.SplatNum = Math::Clamp(buffer->Data.SplatNum, 0, 3);
	}

	string str;
	if (texture[buffer->Data.SplatNum] != nullptr)
		str = String::WStringToString(Path::GetFileName(texture[buffer->Data.SplatNum]->GetFile()));
	else
		str = "None";

	ImGuiHelper::RenderImageButton(&texture[buffer->Data.SplatNum],ImVec2(100,100));


}

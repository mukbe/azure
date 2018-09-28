#include "stdafx.h"
#include "Terrain.h"
#include "TerrainHeight.h"
#include "TerrainSplat.h"
#include "TerrainSmooth.h"

Terrain::Terrain()
	:height(255), width(255)
{
	this->name = "Terrain";

	CreateTerrain();
	shader = Shaders->CreateShader("TerrainTool", L"TerrainTool.hlsl", Shader::ShaderType::useHS, "TerrainTool");
	
	worldBuffer = Buffers->FindShaderBuffer<WorldBuffer>();
	heightData = new TerrainHeight;

	texture = new Texture(Contents + L"Dirt2.png");

	buffer = new Buffer;

	vector<D3DXVECTOR4>temp;
	temp.assign(10, D3DXVECTOR4(0,0,0,FLT_MAX));
	pickPos = new CResource1D(sizeof(D3DXVECTOR4), 10, temp.data());

	computePicking = new ComputeShader(ShaderPath + L"TerrainToolCS.hlsl", "CalcuPickPosition");



	splat = new TerrainSplat;
	smooth = new TerrainSmooth;
}


Terrain::~Terrain()
{
	SafeDelete(buffer);
	SafeDelete(texture);
	SafeDelete(heightData);


	SafeDelete(computePicking);
	SafeDelete(pickPos);

	SafeDelete(splat);
}

void Terrain::PreUpdate()
{
	CalcuMousePosition();

}

void Terrain::Update()
{
	

}

void Terrain::PostUpdate()
{
}

void Terrain::PreRender()
{
}

void Terrain::Render()
{

	UINT stride = sizeof(VertexTextureNormal);
	UINT offset = 0;

	DeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	DeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);
	worldBuffer->SetMatrix(mat);

	worldBuffer->SetVSBuffer(1);
	worldBuffer->SetDSBuffer(1);
	buffer->SetVSBuffer(5);
	buffer->SetPSBuffer(5);

	ID3D11ShaderResourceView* heightMap = heightData->GetHeightMap();
	DeviceContext->DSSetShaderResources(0, 1, &heightMap);
	
	ID3D11ShaderResourceView* textureView = texture->GetSRV();
	DeviceContext->PSSetShaderResources(0, 1, &textureView);


	shader->Render();
	//Splat
	splat->Render();

	States::SetSampler(1, States::LINEAR_WRAP);
	DeviceContext->DrawIndexed(indexData.size(), 0, 0);

	//Release
	shader->ReleaseShader();
	ID3D11ShaderResourceView* nullsrv[1] = { nullptr };
	DeviceContext->DSSetShaderResources(0, 1, nullsrv);
	DeviceContext->PSSetShaderResources(0, 1, nullsrv);
}

void Terrain::UIRender()
{
	//ImGui::Begin("Terrain");

	if (ImGui::Button("Save", ImVec2(150, 30)))
	{
		//Save
		Texture::SaveToFile(Contents + L"heightTestMap.png", heightData->GetHeightBuffer()->GetSRV());
		Texture::SaveToFile(Contents + L"splatMap.png", splat->GetSplatMap()->GetSRV());

		//heightRatio = 250.f

		buffer->SetCSBuffer(1);
		heightData->GetHeightBuffer()->BindCSShaderResourceView(0);
		heightData->GetTempHeightBuffer()->BindResource(2);
		smooth->CopyHeight();
		heightData->GetHeightBuffer()->ReleaseCSshaderResorceView(0);
		vector<D3DXVECTOR4> heightColor;
		heightColor.assign(256 * 256, D3DXVECTOR4());
		heightData->GetTempHeightBuffer()->GetDatas(heightColor.data());

		for (size_t i = 0; i < vertexData.size(); i++)
			vertexData[i].position.y = heightColor[i].x * 250.f;

		Math::ComputeNormal(vertexData, normalIndexData);
		Texture* normalData = new Texture(DXGI_FORMAT_R8G8B8A8_UNORM);

		vector<D3DXCOLOR> data;
		data.assign(256 * 256, D3DXCOLOR(0,0,0,1));
		for (size_t i = 0; i < vertexData.size(); i++)
		{
			memcpy(&data[i], &vertexData[i].normal, sizeof(D3DXVECTOR3));
		}

		normalData->SetPixel(data, 256, 256);
		Texture::SaveToFile(Contents + L"normalMap.png", normalData->GetSRV());



	}
	ImGui::SameLine();
	if (ImGui::Button("Load", ImVec2(150, 30)))
	{
		//Load
		Texture* tex = new Texture(Contents + L"heightTestMap.png");
		tex->SetCSResource(0);
		heightData->Load();
		SafeDelete(tex);
	}
	ImGui::Separator();

	const char*  str[4] = { "None","Height","Splat","Smooth" };
	static int modeCheck = 0;
	for (int i = 0; i < Mode::Mode_End; i++)
	{
		if (ImGui::RadioButton(str[i], &modeCheck, i))
		{
			mode = Mode(i);
		}
		if (i != Mode::Mode_End - 1)
			ImGui::SameLine();

	}

	ImGui::SliderInt("BrushStyle", &buffer->Data.BrushStyle, 0, 1);
	ImGui::SliderFloat("BrushSize", &buffer->Data.BrushSize, 0.5f, 40.f);

	ImGui::SliderFloat("GridThickness", &buffer->Data.GridThickness, 0.001f, 1.0f);
	ImGui::Checkbox("GridView", (bool*)&buffer->Data.GridbView);


	switch (mode)
	{
		case Mode::Mode_None:
		break;
		case Mode::Mode_Height:
		ImGui::SliderFloat("HeightAmount", &buffer->Data.HeightEditAmount, -0.05f, 0.05f);

		break;
		case Mode::Mode_Splat:
		{
			splat->TerrainUI();
		}
		break;
		case Mode::Mode_Smooth:
		break;
	}


	//ImGui::End();
}

void Terrain::UIUpdate()
{
	if (Mouse::Get()->Press(0) && Keyboard::Get()->Press(VK_CONTROL))
	{
		switch (mode)
		{
		case Mode::Mode_None:
			break;
		case Mode::Mode_Height:
		{
			buffer->SetCSBuffer(1);
			heightData->GetHeightBuffer()->BindCSShaderResourceView(0);
			heightData->CopyHeight();
			heightData->GetHeightBuffer()->ReleaseCSshaderResorceView(0);

			heightData->GetHeightBuffer()->BindResource(1);
			heightData->EditHeight();
			heightData->GetHeightBuffer()->ReleaseResource(1);

		}
		break;
		case Mode::Mode_Splat:
		{
			buffer->SetCSBuffer(1);
			heightData->GetHeightBuffer()->BindResource(1);
			splat->Splat();
			heightData->GetHeightBuffer()->ReleaseResource(1);
		}
		break;
		case Mode::Mode_Smooth:
		{
			//TODO 먹통이 됬음 해결좀
			buffer->SetCSBuffer(1);
			heightData->GetHeightBuffer()->BindCSShaderResourceView(0);
			heightData->GetTempHeightBuffer()->BindResource(2);
			smooth->CopyHeight();
			heightData->GetHeightBuffer()->ReleaseCSshaderResorceView(0);

			heightData->GetHeightBuffer()->BindResource(1);
			smooth->EditHeight();
			heightData->GetTempHeightBuffer()->ReleaseResource(2);
			heightData->GetHeightBuffer()->ReleaseResource(1);
		}
		break;
		}
	}
	//if (Keyboard::Get()->Down(VK_F1))
	//{
	//	//Save
	//	Texture::SaveToFile(Contents + L"heightTestMap.png", heightData->GetHeightBuffer()->GetSRV());
	//	Texture::SaveToFile(Contents + L"splatMap.png", splat->GetSplatMap()->GetSRV());
	//}
	//if (Keyboard::Get()->Down(VK_F2))
	//{
	//	Texture* tex = new Texture(Contents + L"heightTestMap.png");
	//	tex->SetCSResource(0);
	//	heightData->Load();
	//	//TODO  splat Load
	//}
}

void Terrain::CreateTerrain()
{
	UINT vertexCount = (width + 1) * (height + 1);
	vertexData.assign(vertexCount, VertexTextureNormal());

	UINT index = 0;
	for (UINT z = 0; z <= height; z++)
	{
		for (UINT x = 0; x <= width; x++)
		{
			vertexData[index].position.x = (float)x;
			vertexData[index].position.y = (float)0.f;
			vertexData[index].position.z = (float)z;

			vertexData[index].uv.x = x / (float)width;
			vertexData[index].uv.y = z / (float)height;

			vertexData[index].normal = D3DXVECTOR3(0, 1, 0);

			index++;
		}//for(x)
	}//for(y)


	UINT indexCount = width * height * 4;
	indexData.assign(indexCount, UINT());

	normalIndexData.assign(indexCount / 4 * 6, UINT());

	index = 0;
	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			indexData[index + 0] = (width + 1) * z + x;
			indexData[index + 1] = (width + 1) * (z + 1) + x;
			indexData[index + 2] = (width + 1) * z + x + 1;
			indexData[index + 3] = (width + 1) * (z + 1) + (x + 1);


			index += 4;
		}
	}

	index = 0;
	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			normalIndexData[index + 0] = (width + 1) * z + x;
			normalIndexData[index + 1] = (width + 1) * (z + 1) + x;
			normalIndexData[index + 2] = (width + 1) * z + x + 1;
			normalIndexData[index + 3] = (width + 1) * z + x + 1;
			normalIndexData[index + 4] = (width + 1) * (z + 1) + x;
			normalIndexData[index + 5] = (width + 1) * (z + 1) + (x + 1);


			index += 6;
		}
	}



	
	//VertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTextureNormal) * vertexData.size();
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &vertexData[0];

		HRESULT hr;
		hr = Device->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//IndexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * indexData.size();
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data = { 0 };
		data.pSysMem = &indexData[0];

		HRESULT hr;
		hr = Device->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}

}

void Terrain::CalcuMousePosition()
{
	D3DDesc desc;
	DxRenderer::GetDesc(&desc);
	D3DXVECTOR2 screenSize;
	screenSize.x = (float)desc.Width;
	screenSize.y = (float)desc.Height;

	D3DXVECTOR3 mouse = Mouse::Get()->GetPosition();

	D3DXVECTOR2 point;
	point.x = ((2.0f * mouse.x) / screenSize.x) - 1.0f;
	point.y = (((2.0f * mouse.y) / screenSize.y) - 1.0f) * -1.0f;

	buffer->Data.MouseScreenPos = point;

	buffer->SetCSBuffer(1);
	computePicking->BindShader();
	pickPos->BindResource(0);
	ID3D11ShaderResourceView* heightMap = heightData->GetHeightMap();
	DeviceContext->CSSetShaderResources(0, 1, &heightMap);

	computePicking->Dispatch(16, 16, 1);

	//Release
	pickPos->ReleaseResource(0);
	ID3D11ShaderResourceView* nullsrv[1] = { nullptr };
	DeviceContext->CSSetShaderResources(0, 1, nullsrv);

	vector<D3DXVECTOR4>temp;
	temp.assign(10, D3DXVECTOR4());
	pickPos->GetDatas(&temp[0]);

	for (size_t i = 0; i < temp.size(); i++)
	{
		if (temp[0].w > temp[i].w && temp[i].w !=0)
		{
			D3DXVECTOR4 tt;
			tt = temp[0];
			temp[0] = temp[i];
			temp[i] = tt;
		}
	}

	buffer->Data.PickPos = D3DXVECTOR3(temp[0].x, temp[0].y, temp[0].z);
}


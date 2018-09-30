#include "stdafx.h"
#include "GameMap.h"

#include "./Bounding/QuadTree/QuadTreeSystem.h"

GameMap::GameMap(string level)
{
	name = "Terrain";

	//Json::Value* jsonRoot = new Json::Value();
	//JsonHelper::ReadData(String::StringToWString(level), jsonRoot);
	//Json::Value prop = (*jsonRoot)["Textures"];
	//if (prop.isNull() == false)
	//{
	//	Texture* splatTex[4];
	//	string heightMapPath, splatMapPath, normalMapPath;
	//	string splatTexPath[4], diffusePath;
	//
	//	JsonHelper::GetValue(prop, "HeightMap", heightMapPath);
	//	JsonHelper::GetValue(prop, "SplatMap", splatMapPath);
	//	JsonHelper::GetValue(prop, "NormalMap", normalMapPath);
	//
	//	JsonHelper::GetValue(prop, "splat0", diffusePath);
	//	JsonHelper::GetValue(prop, "splat1", splatTexPath[0]);
	//	JsonHelper::GetValue(prop, "splat2", splatTexPath[1]);
	//	JsonHelper::GetValue(prop, "splat3", splatTexPath[2]);
	//	JsonHelper::GetValue(prop, "splat4", splatTexPath[3]);
	//
	//	heightMap = new Texture(Contents + String::StringToWString(heightMapPath));
	//	splatMap = new Texture(Contents + String::StringToWString(splatMapPath));
	//	normalMap = new Texture(Contents + String::StringToWString(normalMapPath));
	//
	//	diffuseMap = new Texture(Contents + String::StringToWString(diffusePath));
	//
	//	for (int i = 0; i < 4; i++)
	//	{
	//		if (splatTexPath[i] == "")
	//			splatTex[i] = nullptr;
	//		
	//		else
	//			splatTex[i] = new Texture(Contents + String::StringToWString(splatTexPath[i]));
	//	}
	//
	//	UINT index = 0;
	//	for (int i = 0; i < 4; i++)
	//	{
	//		if (splatTex[i] != nullptr)
	//		{
	//			splatTexView[index] = splatTex[i]->GetSRV();
	//			index++;
	//		}
	//	}
	//}

	heightMap = new Texture(Contents + L"heightTestMap.png");
	splatMap = new Texture(Contents + L"splatMap.png");
	normalMap = new Texture(Contents +L"normalMap.png");


	//heightMap받아와서 정점에 넣어주고 normal은 노멀맵으로 대체한다 uv값은 gpu static에 있는걸 사용
	width = height = 32;
	scale = 8;
	vertexData.assign((width+1) * (height+1), VertexTextureNormal());
	indexData.assign(width * height * 4,UINT());

	UINT index = 0;
	for (UINT z = 0; z <= height; z++)
	{
		for (UINT x = 0; x <= width; x++)
		{
			vertexData[index].position.x = (float)x * scale;
			vertexData[index].position.y = (float)0.f;
			vertexData[index].position.z = (float)z * scale;

			vertexData[index].uv.x = x / (float)width;
			vertexData[index].uv.y = z / (float)height;

			vertexData[index].normal = D3DXVECTOR3(0, 1, 0);

			index++;
		}//for(x)
	}//for(y)

	index = 0;
	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			indexData[index + 0] = (width + 1) * z + x;
			indexData[index + 1] = (width + 1) * (z + 1) + x;
			indexData[index + 2] = (width + 1) * z + (x + 1);
			indexData[index + 3] = (width + 1) *  (z + 1) + (x + 1);


			index += 4;
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

	worldBuffer = Buffers->FindShaderBuffer<WorldBuffer>();

	shader = Shaders->CreateShader("Terrain", L"999_Terrain.hlsl", Shader::ShaderType::useHS, "Terrain");

	buffer = new Buffer;
	buffer->Data.Edge = 16;
	buffer->Data.Inside = 8;
}


GameMap::~GameMap()
{
}

void GameMap::Render()
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
	buffer->SetHSBuffer(5);
	buffer->SetDSBuffer(5);

	//Bind Textures
	ID3D11ShaderResourceView* heightView = heightMap->GetSRV();
	DeviceContext->DSSetShaderResources(0, 1, &heightView);
	ID3D11ShaderResourceView* normalView = normalMap->GetSRV();
	DeviceContext->DSSetShaderResources(1, 1, &normalView);
	ID3D11ShaderResourceView* splatView = splatMap->GetSRV();
	DeviceContext->PSSetShaderResources(5, 1, &splatView);

	//DeviceContext->PSSetShaderResources(6, 4, splatTexView);

	shader->Render();
	States::SetRasterizer(States::RasterizerStates::WIRE_CULL_OFF);
	{
		States::SetSampler(1, States::LINEAR_WRAP);
		DeviceContext->DrawIndexed(indexData.size(), 0, 0);
	}
	States::SetRasterizer(States::RasterizerStates::SOLID_CULL_ON);

	//Release
	shader->ReleaseShader();
	ID3D11ShaderResourceView* nullsrv[1] = { nullptr };
	DeviceContext->DSSetShaderResources(0, 1, nullsrv);
	DeviceContext->PSSetShaderResources(0, 1, nullsrv);

}

void GameMap::UIRender()
{

}

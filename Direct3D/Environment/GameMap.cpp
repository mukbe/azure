#include "stdafx.h"
#include "GameMap.h"

#include "./Bounding/QuadTree/QuadTreeSystem.h"

GameMap::GameMap(string level)
{
	name = "Terrain";


	//Json::Value* jsonRoot = new Json::Value();
	//JsonHelper::ReadData(String::StringToWString(level), jsonRoot);
	//Json::Value prop = (*jsonRoot)[name.c_str()];
	//if (prop.isNull() == false)
	//{
	//	Texture* splatTex[4];
	//	string heightMapPath, splatMapPath, normalMapPath;
	//	string splatTexPath[4], diffusePath;

	//	JsonHelper::GetValue(prop, "HeightMap", heightMapPath);
	//	JsonHelper::GetValue(prop, "SplatMap", splatMapPath);
	//	JsonHelper::GetValue(prop, "NormalMap", normalMapPath);

	//	JsonHelper::GetValue(prop, "Splat0", diffusePath);
	//	JsonHelper::GetValue(prop, "Splat1", splatTexPath[0]);
	//	JsonHelper::GetValue(prop, "Splat2", splatTexPath[1]);
	//	JsonHelper::GetValue(prop, "Splat3", splatTexPath[2]);
	//	JsonHelper::GetValue(prop, "Splat4", splatTexPath[3]);

	//	heightMap = new Texture( String::StringToWString(heightMapPath));
	//	splatMap = new Texture( String::StringToWString(splatMapPath));
	//	normalMap = new Texture( String::StringToWString(normalMapPath));

	//	diffuseMap = new Texture( String::StringToWString(diffusePath));

	//	for (int i = 0; i < 4; i++)
	//	{
	//		if (splatTexPath[i] == "")
	//			splatTex[i] = nullptr;
	//		
	//		else
	//			splatTex[i] = new Texture(Contents + String::StringToWString(splatTexPath[i]));
	//	}

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

	//heightMap = new Texture(Contents + L"HeightMap.png");
	//splatMap = new Texture(Contents + L"SplatMap.png");
	//normalMap = new Texture(Contents +L"NormalMap.png");


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
	shadowShader = Shaders->CreateShader("TerrainShadow", L"999_TerrainShadow.hlsl", Shader::ShaderType::useHS, "TerrainShadow");
	buffer = new Buffer;
	buffer->Data.Edge = 16;
	buffer->Data.Inside = 8;
}


GameMap::~GameMap()
{
}

void GameMap::Render()
{
	//wireFrame = true;
	if (wireFrame)
		States::SetRasterizer(States::RasterizerStates::WIRE_CULL_ON);

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
	Cameras->BindGPU("FreeCamera");

	//Bind Textures
	ID3D11ShaderResourceView* heightView = heightMap->GetSRV();
	DeviceContext->DSSetShaderResources(0, 1, &heightView);
	ID3D11ShaderResourceView* normalView = normalMap->GetSRV();
	DeviceContext->DSSetShaderResources(1, 1, &normalView);
	ID3D11ShaderResourceView* splatView = splatMap->GetSRV();
	DeviceContext->PSSetShaderResources(5, 1, &splatView);
	ID3D11ShaderResourceView* diffuseView = diffuseMap->GetSRV();
	DeviceContext->PSSetShaderResources(0, 1, &diffuseView);

	DeviceContext->PSSetShaderResources(6, 4, splatTexView);

	shader->Render();
	States::SetSampler(1, States::LINEAR_WRAP);
	DeviceContext->DrawIndexed(indexData.size(), 0, 0);

	//Release
	shader->ReleaseShader();
	ID3D11ShaderResourceView* nullsrv[1] = { nullptr };
	DeviceContext->DSSetShaderResources(0, 1, nullsrv);
	DeviceContext->PSSetShaderResources(0, 1, nullsrv);

	if (wireFrame)
		States::SetRasterizer(States::RasterizerStates::SOLID_CULL_ON);

}

void GameMap::UIRender()
{
	ImGui::Checkbox("WireFrame", &wireFrame);
	
	if (ImGui::InputInt("Edge", &buffer->Data.Edge))
	{
		buffer->Data.Inside = buffer->Data.Edge * 0.5f;
	}
	ImGuiInputTextFlags flag = 0;
	flag |= ImGuiInputTextFlags_ReadOnly;
	ImGui::InputInt("Inside", &buffer->Data.Inside, flag);
}

void GameMap::LoadData(Json::Value * json)
{
	string file;
	JsonHelper::GetValue(*json, "FileName", file);

	Json::Value* jsonRoot = new Json::Value();
	JsonHelper::ReadData(String::StringToWString(file), jsonRoot);
	Json::Value prop = (*jsonRoot)["Property"];
	if (prop.isNull() == false)
	{
		Texture* splatTex[4];
		string heightMapPath, splatMapPath, normalMapPath;
		string splatTexPath[4], diffusePath;

		JsonHelper::GetValue(prop, "HeightMap", heightMapPath);
		JsonHelper::GetValue(prop, "SplatMap", splatMapPath);
		JsonHelper::GetValue(prop, "NormalMap", normalMapPath);

		JsonHelper::GetValue(prop, "Splat0", diffusePath);
		JsonHelper::GetValue(prop, "Splat1", splatTexPath[0]);
		JsonHelper::GetValue(prop, "Splat2", splatTexPath[1]);
		JsonHelper::GetValue(prop, "Splat3", splatTexPath[2]);
		JsonHelper::GetValue(prop, "Splat4", splatTexPath[3]);

		heightMap = new Texture(Contents + String::StringToWString(heightMapPath));
		splatMap = new Texture(Contents + String::StringToWString(splatMapPath));
		normalMap = new Texture(Contents + String::StringToWString(normalMapPath));

		diffuseMap = new Texture(String::StringToWString(diffusePath));

		for (int i = 0; i < 4; i++)
		{
			if (splatTexPath[i] == "")
				splatTex[i] = nullptr;

			else
				splatTex[i] = new Texture(Contents + String::StringToWString(splatTexPath[i]));
		}

		for (int i = 0; i < 4; i++)
		{
			if (splatTex[i] != nullptr)
				splatTexView[i] = splatTex[i]->GetSRV();
			else
				splatTexView[i] = nullptr;
		}
	}
}

void GameMap::ShadowRender()
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

	shadowShader->Render();
	States::SetSampler(0, States::LINEAR);
	DeviceContext->DrawIndexed(indexData.size(), 0, 0);

	//Release
	shadowShader->ReleaseShader();
	ID3D11ShaderResourceView* nullsrv[1] = { nullptr };
	DeviceContext->DSSetShaderResources(0, 1, nullsrv);
	DeviceContext->PSSetShaderResources(0, 1, nullsrv);

}

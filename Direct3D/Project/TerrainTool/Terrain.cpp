#include "stdafx.h"
#include "Terrain.h"

#include "./Renders/WorldBuffer.h"

#include "TerrainHeight.h"

#include "./Renders/Texture.h"
Terrain::Terrain()
	:height(256), width(256)
{
	CreateTerrain();
	shader = Shaders->CreateShader("TerrainTool", L"TerrainTool.hlsl", Shader::ShaderType::useHS, "TerrainTool");
	
	worldBuffer = Buffers->FindShaderBuffer<WorldBuffer>();
	heightData = new TerrainHeight;

	texture = new Texture(Contents + L"Dirt.png");

}


Terrain::~Terrain()
{
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

	worldBuffer->SetVSBuffer(1);
	worldBuffer->SetDSBuffer(1);

	ID3D11ShaderResourceView* heightMap = heightData->GetHeightMap();
	DeviceContext->DSSetShaderResources(0, 1, &heightMap);
	
	ID3D11ShaderResourceView* textureView = texture->GetSRV();
	DeviceContext->PSSetShaderResources(0, 1, &textureView);


	shader->Render();
	
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

	index = 0;
	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			//indexData[index + 0] = (width + 1) * z + x;
			//indexData[index + 1] = (width + 1) * (z + 1) + x;
			//indexData[index + 2] = (width + 1) * z + x + 1;
			//indexData[index + 3] = (width + 1) * z + x + 1;
			//indexData[index + 4] = (width + 1) * (z + 1) + x;
			//indexData[index + 5] = (width + 1) * (z + 1) + (x + 1);

			indexData[index + 0] = (width + 1) * z + x;
			indexData[index + 1] = (width + 1) * (z + 1) + x;
			indexData[index + 2] = (width + 1) * z + x + 1;
			indexData[index + 3] = (width + 1) * (z + 1) + (x + 1);


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

}

void Terrain::CalcuMousePosition()
{

}

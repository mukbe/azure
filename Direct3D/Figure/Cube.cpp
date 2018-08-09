#include "stdafx.h"
#include "Cube.h"

#include "./Utilities/GeometryGenerator.h"
#include "./Utilities/Buffer.h"
#include "./Renders/Shader.h"
#include "./Renders/WorldBuffer.h"

Cube::Cube()
{
	GeometryGenerator::MeshData data;
	
	GeometryGenerator geo;
	geo.CreateBox(10.f, 10.f, 10.f, data);

	VertexColor* vertex = new VertexColor[data.Vertices.size()];

	for (UINT i = 0; i < data.Vertices.size(); ++i)
	{
		vertex[i].position = data.Vertices[i].Position;
		vertex[i].color = D3DXCOLOR(0.f, 0.f, 1.f, 1.f);
	}
	
	Buffer::CreateVertexBuffer(&vertexBuffer, vertex, sizeof VertexColor * data.Vertices.size());
	Buffer::CreateIndexBuffer(&indexBuffer, &data.Indices[0], data.Indices.size());

	this->indexCount = data.Indices.size();

	SafeDeleteArray(vertex);
	this->shader = new Shader(Shaders + L"001_Color.hlsl");

	this->worldBuffer = new WorldBuffer;

	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof D3D11_RASTERIZER_DESC);
	desc.CullMode = D3D11_CULL_NONE;
	desc.FillMode = D3D11_FILL_SOLID;
	_Device->CreateRasterizerState(&desc, &renderState);
}


Cube::~Cube()
{
	SafeRelease(vertexBuffer);
	SafeRelease(indexBuffer);

	SafeDelete(shader);
	SafeDelete(worldBuffer);
}

void Cube::Render()
{
	//_Context->RSSetState(renderState);
	
	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);
	worldBuffer->SetMatrix(mat);

	UINT stride = sizeof VertexColor;
	UINT offset = 0;

	_Context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	_Context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT,0);
	_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->Render();

	worldBuffer->SetVSBuffer(1);

	_Context->DrawIndexed(indexCount, 0, 0);
}

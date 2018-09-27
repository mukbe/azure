#include "stdafx.h"
#include "Figure.h"

#include "./Utilities/Transform.h"
#include "./Renders/WorldBuffer.h"
#include "./Utilities/GeometryGenerator.h"
#include "./Utilities/Buffer.h"

Figure::Figure(FigureType type, float radius,D3DXCOLOR color)
{
	GeometryGenerator::MeshData data;
	GeometryGenerator geo;

	if (type == FigureType::Box)
	{
		geo.CreateBox(radius, radius, radius, data);
		vector<VertexType> vertex;
		vertex.assign(data.Vertices.size(), VertexColorNormal());

		for (UINT i = 0; i < data.Vertices.size(); ++i)
		{
			vertex[i].position = data.Vertices[i].Position;
			vertex[i].normal = data.Vertices[i].Normal;
			vertex[i].color = color;
		}

		this->indexCount = data.Indices.size();
		Buffer::CreateVertexBuffer(&vertexBuffer, &vertex[0], sizeof VertexType * data.Vertices.size());
		Buffer::CreateIndexBuffer(&indexBuffer, &data.Indices[0], indexCount);
	}

	else if (type == FigureType::Grid)
	{
		vector<VertexType> vertex;
		vertex.assign(4, VertexColorNormal());

		vertex[0].position = D3DXVECTOR3(0.f, 0.f, 0.f);
		vertex[1].position = D3DXVECTOR3(0.f, 0.f, radius);
		vertex[2].position = D3DXVECTOR3(radius, 0.f, radius);
		vertex[3].position = D3DXVECTOR3(radius, 0.f, 0.f);

		for (UINT i = 0; i < 4; ++i)
			vertex[i].normal = D3DXVECTOR3(0.f, 1.f, 0.f);
		vertex[0].color = vertex[1].color = vertex[2].color = vertex[3].color = color;

		vector<UINT> index;
		index.assign(6, UINT());
		index[0] = 0;
		index[1] = 1;
		index[2] = 3;
		index[3] = 1;
		index[4] = 2;
		index[5] = 3;

		this->indexCount = 6;
		Buffer::CreateVertexBuffer(&vertexBuffer, &vertex[0], sizeof VertexType * vertex.size());
		Buffer::CreateIndexBuffer(&indexBuffer, &index[0], indexCount);
	}
	else
	{
		geo.CreateSphere(radius, 50, 50, data);
		vector<VertexType> vertex;
		vertex.assign(data.Vertices.size(), VertexColorNormal());

		for (UINT i = 0; i < data.Vertices.size(); ++i)
		{
			vertex[i].position = data.Vertices[i].Position;
			vertex[i].normal = data.Vertices[i].Normal;
			vertex[i].color = color;
		}

		this->indexCount = data.Indices.size();
		Buffer::CreateVertexBuffer(&vertexBuffer, &vertex[0], sizeof VertexType * data.Vertices.size());
		Buffer::CreateIndexBuffer(&indexBuffer, &data.Indices[0], indexCount);
	}


	shader = Shaders->FindShader("colorGBuffer");
	//shader = new Shader(ShaderPath + L"007_Alpha.hlsl");
	worldBuffer = Buffers->FindShaderBuffer<WorldBuffer>();
	transform = new Transform;

	shadowShader = Shaders->FindShader("colorShadow");
}

Figure::~Figure()
{
	SafeDelete(transform);

	SafeRelease(vertexBuffer);
	SafeRelease(indexBuffer);
}

void Figure::Render()
{
	UINT stride = sizeof VertexType;
	UINT offset = 0;

	DeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	DeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->Render();

	worldBuffer->SetMatrix(transform->GetFinalMatrix());
	worldBuffer->SetVSBuffer(1);

	DeviceContext->DrawIndexed(indexCount, 0, 0);
}

void Figure::ShadowRender()
{
	UINT stride = sizeof VertexType;
	UINT offset = 0;

	DeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	DeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shadowShader->Render();

	worldBuffer->SetMatrix(transform->GetFinalMatrix());
	worldBuffer->SetVSBuffer(1);

	DeviceContext->DrawIndexed(indexCount, 0, 0);
}

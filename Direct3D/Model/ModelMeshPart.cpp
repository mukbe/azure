#include "stdafx.h"
#include "ModelMeshPart.h"

#include "../Renders/Material.h"

void ModelMeshPart::Clone(void ** clone)
{
	ModelMeshPart* part = new ModelMeshPart();
	part->materialName = materialName;

	part->vertices.assign(vertices.begin(), vertices.end());
	part->indices.assign(indices.begin(), indices.end());

	*clone = part;
}

ModelMeshPart::ModelMeshPart()
	:vertexBuffer(nullptr),indexBuffer(nullptr)
{
	material = new Material;
}


ModelMeshPart::~ModelMeshPart()
{
	SafeRelease(vertexBuffer);
	SafeRelease(indexBuffer);

	vertices.clear();
	indices.clear();

	SafeDelete(material);
}

void ModelMeshPart::Binding()
{
	HRESULT hr;
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;

	//1. Vertex Buffer
	{
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexType) * vertices.size();
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = &vertices[0];

		hr = Device->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//2. Index Buffer
	{
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * indices.size();
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = &indices[0];

		hr = Device->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}
}

void ModelMeshPart::Render(Shader* shader)
{
	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	material->UpdateBuffer();
	material->BindBuffer();

	DeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	DeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->Render();

	DeviceContext->DrawIndexed(indices.size(), 0, 0);

	material->UnBindBuffer();
}

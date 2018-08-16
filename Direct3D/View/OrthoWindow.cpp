#include "stdafx.h"
#include "OrthoWindow.h"

#include "./Renders/WorldBuffer.h"

OrthoWindow::OrthoWindow(int windowWidth, int windowHeight)
{
	// ������ ������ ȭ�� ��ǥ�� ����մϴ�.
	float left = (float)((windowWidth / 2) * -1);

	// ������ �������� ȭ�� ��ǥ�� ����մϴ�.
	float right = left + (float)windowWidth;

	// ������ ����� ȭ�� ��ǥ�� ����մϴ�.
	float top = (float)(windowHeight / 2);

	// ������ �ϴ��� ȭ�� ��ǥ�� ����մϴ�.
	float bottom = top - (float)windowHeight;

	// ���� �迭�� ���� ���� �����մϴ�.
	this->vertexCount = 6;

	// �ε��� �迭�� �ε��� ���� �����մϴ�.
	indexCount = vertexCount;

	// ���� �迭�� ����ϴ�.
	VertexType* vertices = new VertexType[vertexCount];


	// �ε��� �迭�� ����ϴ�.
	UINT* indices = new UINT[indexCount];

	// ���� �迭�� �����͸��ε��մϴ�.
	//// ù ��° �ﰢ��.
	vertices[0].position = D3DXVECTOR3(left, top, 0.0f);  // ���� ��
	vertices[0].uv = D3DXVECTOR2(0.0f, 0.0f);

	vertices[1].position = D3DXVECTOR3(right, bottom, 0.0f);  // ������ �Ʒ�
	vertices[1].uv = D3DXVECTOR2(1.0f, 1.0f);

	vertices[2].position = D3DXVECTOR3(left, bottom, 0.0f);  // ���� �Ʒ�
	vertices[2].uv = D3DXVECTOR2(0.0f, 1.0f);

	// �� ��° �ﰢ��.
	vertices[3].position = D3DXVECTOR3(left, top, 0.0f);  // ���� ��
	vertices[3].uv = D3DXVECTOR2(0.0f, 0.0f);

	vertices[4].position = D3DXVECTOR3(right, top, 0.0f);  // ������ ��
	vertices[4].uv = D3DXVECTOR2(1.0f, 0.0f);

	vertices[5].position = D3DXVECTOR3(right, bottom, 0.0f);  // ������ �Ʒ�
	vertices[5].uv = D3DXVECTOR2(1.0f, 1.0f);


	// �����ͷ� �ε��� �迭���ε��մϴ�.
	for (int i = 0; i<indexCount; i++)
	{
		indices[i] = i;
	}

	// ���� ���� ������ ������ �����Ѵ�.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource ������ ���� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	Device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// ���� �ε��� ������ ������ �����մϴ�.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// ���� ���ҽ� ������ �ε��� �����Ϳ� ���� �����͸� �����մϴ�.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	Device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	// ���� ���ؽ��� �ε��� ���۰� �����ǰ�ε� �� �迭�� �����Ͻʽÿ�.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	D3DXMatrixOrthoLH(&projection, (float)windowWidth, (float)windowHeight, 0.f, 1000.0f);
	//viewProjectionBuffer->SetOrtho(projection);

}

OrthoWindow::~OrthoWindow()
{
	SafeRelease(vertexBuffer);
	SafeRelease(indexBuffer);
}

void OrthoWindow::Render()
{
	// ���� ���� ���� �� �������� �����մϴ�.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// ������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ������ �����մϴ�.
	DeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// ������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ������ �����մϴ�.
	DeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// �� ������ ���ۿ��� �������Ǿ���ϴ� ������Ƽ�� ������ �����մϴ�.�� ��쿡�� �ﰢ���Դϴ�.
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//viewProjectionBuffer->SetVSBuffer(0);
	//viewProjectionBuffer->SetPSBuffer(0);

}

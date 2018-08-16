#include "stdafx.h"
#include "OrthoWindow.h"

#include "./Renders/WorldBuffer.h"

OrthoWindow::OrthoWindow(int windowWidth, int windowHeight)
{
	// 윈도우 왼쪽의 화면 좌표를 계산합니다.
	float left = (float)((windowWidth / 2) * -1);

	// 윈도우 오른쪽의 화면 좌표를 계산합니다.
	float right = left + (float)windowWidth;

	// 윈도우 상단의 화면 좌표를 계산합니다.
	float top = (float)(windowHeight / 2);

	// 윈도우 하단의 화면 좌표를 계산합니다.
	float bottom = top - (float)windowHeight;

	// 정점 배열의 정점 수를 설정합니다.
	this->vertexCount = 6;

	// 인덱스 배열의 인덱스 수를 설정합니다.
	indexCount = vertexCount;

	// 정점 배열을 만듭니다.
	VertexType* vertices = new VertexType[vertexCount];


	// 인덱스 배열을 만듭니다.
	UINT* indices = new UINT[indexCount];

	// 정점 배열에 데이터를로드합니다.
	//// 첫 번째 삼각형.
	vertices[0].position = D3DXVECTOR3(left, top, 0.0f);  // 왼쪽 위
	vertices[0].uv = D3DXVECTOR2(0.0f, 0.0f);

	vertices[1].position = D3DXVECTOR3(right, bottom, 0.0f);  // 오른쪽 아래
	vertices[1].uv = D3DXVECTOR2(1.0f, 1.0f);

	vertices[2].position = D3DXVECTOR3(left, bottom, 0.0f);  // 왼쪽 아래
	vertices[2].uv = D3DXVECTOR2(0.0f, 1.0f);

	// 두 번째 삼각형.
	vertices[3].position = D3DXVECTOR3(left, top, 0.0f);  // 왼쪽 위
	vertices[3].uv = D3DXVECTOR2(0.0f, 0.0f);

	vertices[4].position = D3DXVECTOR3(right, top, 0.0f);  // 오른쪽 위
	vertices[4].uv = D3DXVECTOR2(1.0f, 0.0f);

	vertices[5].position = D3DXVECTOR3(right, bottom, 0.0f);  // 오른쪽 아래
	vertices[5].uv = D3DXVECTOR2(1.0f, 1.0f);


	// 데이터로 인덱스 배열을로드합니다.
	for (int i = 0; i<indexCount; i++)
	{
		indices[i] = i;
	}

	// 정적 정점 버퍼의 설명을 설정한다.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	Device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// 정적 인덱스 버퍼의 설명을 설정합니다.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 하위 리소스 구조에 인덱스 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	Device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	// 이제 버텍스와 인덱스 버퍼가 생성되고로드 된 배열을 해제하십시오.
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
	// 정점 버퍼 보폭 및 오프셋을 설정합니다.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	DeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	DeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 이 꼭지점 버퍼에서 렌더링되어야하는 프리미티브 유형을 설정합니다.이 경우에는 삼각형입니다.
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//viewProjectionBuffer->SetVSBuffer(0);
	//viewProjectionBuffer->SetPSBuffer(0);

}

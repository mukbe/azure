#include "stdafx.h"
#include "OrthoWindow.h"

#include "./Utilities/Buffer.h"

/*
0 1 
2 3
*/

OrthoWindow::OrthoWindow(D3DXVECTOR2 pos, D3DXVECTOR2 size)
	:position(pos),size(size)
{	
	UINT vertexCount = 4;
	UINT indexCount = 6;

	vertexData.assign(vertexCount, VertexTexture());
	indexData.assign(indexCount, UINT());

	vertexData[0].position = D3DXVECTOR3(pos.x, pos.y, 0.f);
	vertexData[0].uv = D3DXVECTOR2(0.f, 0.f);

	vertexData[1].position = D3DXVECTOR3(pos.x + size.x, pos.y, 0.f);
	vertexData[1].uv = D3DXVECTOR2(1.f, 0.f);

	vertexData[2].position = D3DXVECTOR3(pos.x, pos.y - size.y, 0.f);
	vertexData[2].uv = D3DXVECTOR2(0.f, 1.f);

	vertexData[3].position = D3DXVECTOR3(pos.x + size.x, pos.y - size.y, 0.f);
	vertexData[3].uv = D3DXVECTOR2(1.f, 1.f);

	indexData[0] = 0;
	indexData[1] = 1;
	indexData[2] = 2;

	indexData[3] = 2;
	indexData[4] = 1;
	indexData[5] = 3;

	Buffer::CreateDynamicVertexBuffer(&vertexBuffer, vertexData.data(), sizeof VertexType * vertexCount);
	Buffer::CreateIndexBuffer(&indexBuffer, indexData.data(), indexCount);

	D3DXMatrixOrthoLH(&projection, size.x, size.y, 0.f, 1000.f);

	buffer = BufferManager::Get()->FindShaderBuffer<OrthoBuffer>();
}

OrthoWindow::~OrthoWindow()
{
	SafeRelease(vertexBuffer);
	SafeRelease(indexBuffer);

	vertexData.clear();
	indexData.clear();
}

void OrthoWindow::UpdateBuffer()
{
	vertexData[0].position = D3DXVECTOR3(position.x, position.y, 0.f);
	vertexData[1].position = D3DXVECTOR3(position.x + size.x, position.y, 0.f);
	vertexData[2].position = D3DXVECTOR3(position.x, position.y - size.y, 0.f);
	vertexData[3].position = D3DXVECTOR3(position.x + size.x, position.y - size.y, 0.f);

	Buffer::UpdateBuffer(&vertexBuffer, vertexData.data(), sizeof VertexType * vertexData.size());
}

void OrthoWindow::Render()
{
	pRenderer->ChangeZBuffer(false);

	// 정점 버퍼 보폭 및 오프셋을 설정합니다.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	DeviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	DeviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 이 꼭지점 버퍼에서 렌더링되어야하는 프리미티브 유형을 설정합니다.이 경우에는 삼각형입니다.
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	buffer->SetVSBuffer(10);
}

void OrthoWindow::DrawIndexed()
{
	DeviceContext->DrawIndexed(6, 0, 0);
	pRenderer->ChangeZBuffer(true);
}

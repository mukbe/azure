#pragma once
#include "Projection.h"
#include "./Renders/ShaderBuffer.h"

struct OrthoBuffer : public ShaderBuffer
{
private:
	struct Struct
	{
		D3DXMATRIX orthoMatrix;
	}data;
public:
	OrthoBuffer()
		:ShaderBuffer(&data, sizeof Struct)
	{
		D3DXMatrixOrthoLH(&data.orthoMatrix, WinSizeX,WinSizeY, 0.f, 1000.f);
		D3DXMatrixTranspose(&data.orthoMatrix, &data.orthoMatrix);
	}
	ShaderBuffer_Mecro(OrthoBuffer)
	
};

class OrthoWindow : public Projection
{
private:
	VertexType(VertexTexture)
public:
	OrthoWindow(D3DXVECTOR2 pos, D3DXVECTOR2 size);		//UI »ý¼ºÀÚ
	~OrthoWindow();

	void UpdateBuffer();

	virtual void Render();
	void DrawIndexed();
private:
	Synthesize(D3DXVECTOR2, position,Position)
	Synthesize(D3DXVECTOR2, size,Size)

	Synthesize(ID3D11Buffer*, vertexBuffer, VertexBuffer)
	Synthesize(ID3D11Buffer*, indexBuffer, IndexBuffer)

	vector<VertexType> vertexData;
	vector<UINT> indexData;
	OrthoBuffer* buffer;
};

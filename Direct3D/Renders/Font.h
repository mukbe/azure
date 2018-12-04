#pragma once

class Font
{
private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Struct))
		{
		}

		struct Struct
		{
			D3DXMATRIX World;
			D3DXMATRIX View;
			D3DXMATRIX Projection;
		}Data;

		void SetProjection(D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX proj)
		{
			//D3DXMatrixTranspose(&Data.WorldPro, &worldProj);
			D3DXMatrixTranspose(&Data.World, &world);
			D3DXMatrixTranspose(&Data.View, &view);
			D3DXMatrixTranspose(&Data.Projection, &proj);
		}
	};

public:
	const UINT MaxSize = 80;
private:
	struct CharsetDesc
	{
		UINT X = 0, Y = 0;
		UINT Width = 0, Height = 0;

		float OffsetX = 0.f, OffsetY = 0.f;
		float AdvanceX = 0.f;
		UINT Page = 0;
	};
	struct FontDesc
	{
		UINT LindeHeight = 0;
		UINT Base = 0;
		UINT Width = 0, Height = 0;
		UINT PageCount = 0;

		CharsetDesc desc[256];
	};
public:
	Font(wstring file, wstring texture);
	~Font();

	void Update();
	void Render();

private:
	void CreateBuffer();
	void Parse(wstring file, wstring texture);

private:
	wstring font;

	Shader* shader;

	D3DXMATRIX view;
	D3DXMATRIX world;
	Buffer* buffer;

	class OrthoWindow* ortho;

	ID3D11Buffer* vertexBuffer, * indexBuffer;
	ID3D11ShaderResourceView* srv;

	VertexTexture* vertices;
	UINT* indices;

	D3DXVECTOR2 testLocation;
	string test;

	FontDesc fontDesc;
	bool bRemap;

	ID3D11BlendState* blendState[2];

};


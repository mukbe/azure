#pragma once
#include "./Object/GameObject/GameObject.h"
class Terrain : public GameObject
{
	enum Mode : int
	{
		Mode_None = 0, Mode_Height, Mode_Splat, Mode_Smooth, Mode_End,
	};
private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Struct))
		{
			Data.BrushStyle = 1;
			Data.BrushSize = 4.0f;
			Data.HeightEditAmount = 0.01f;
			Data.GridbView = 0;
			Data.GridThickness = 1.0f;
		}

		struct Struct
		{
			D3DXVECTOR2 MouseScreenPos;
			int BrushStyle;
			float BrushSize;

			D3DXVECTOR3 PickPos;
			float HeightEditAmount;

			UINT GridbView;
			float GridThickness;
			D3DXVECTOR2 padding;


		}Data;
	};
public:
	Terrain();
	~Terrain();

	void PreUpdate();
	void Update();
	void PostUpdate();

	void PreRender();
	void Render();
	void UIRender();

private:
	void CreateTerrain();
	void CalcuMousePosition();


	vector<VertexTextureNormal> vertexData;
	vector<UINT>indexData;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	UINT height;
	UINT width;

	class WorldBuffer* worldBuffer;
	class Shader* shader;
	class TerrainHeight* heightData;
	class TerrainSplat* splat;
	class TerrainSmooth* smooth;
	Mode mode;

private:
	//지형 텍스처
	class Texture* texture;

private:
	Buffer * buffer;
	ComputeShader* computePicking;
	CResource1D* pickPos;



};



#pragma once
class TerrainSplat
{
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Struct))
		{ 
			Data.SplatNum = 0;
			Data.SplatAmount = 0.005f;
		}

		struct Struct
		{
			int SplatNum;
			float SplatAmount;
			D3DXVECTOR2 SplatBuffer_Padding;

		}Data;
	};


public:
	TerrainSplat();
	~TerrainSplat();
	CResource2D* GetSplatMap() { return splatMap; }

	//TerrainSplat to Compute
	void Splat();

	//Binding Resource
	void Render();

	//Terrain cpp UIRender
	void TerrainUI();
private:
	ComputeShader * computeAlphaSplat;
	CResource2D* splatMap;
	CResource2D* tempMap;

	vector<Texture*> texture;

	Buffer* buffer;
private:
	ComputeShader * computeCopyTex;
};


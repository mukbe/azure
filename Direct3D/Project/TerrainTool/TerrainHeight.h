#pragma once
class TerrainHeight
{
public:
	TerrainHeight();
	~TerrainHeight();


	ID3D11ShaderResourceView* GetHeightMap();
private:
	ComputeShader * heightSet;
	CResource2D* heightMap;

	class Texture* texture;

};


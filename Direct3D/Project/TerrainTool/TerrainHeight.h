#pragma once
class TerrainHeight
{
public:
	TerrainHeight();
	~TerrainHeight();


	ID3D11ShaderResourceView* GetHeightMap();
	CResource2D* GetHeightBuffer() { return heightMap; }
private:
	ComputeShader * heightSet;
	CResource2D* heightMap;

	class Texture* texture;

};


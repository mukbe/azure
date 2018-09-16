#pragma once
class TerrainHeight
{
public:
	TerrainHeight();
	~TerrainHeight();

	void CopyHeight();
	void EditHeight();
	void Load();
	ID3D11ShaderResourceView* GetHeightMap();
	CResource2D* GetHeightBuffer() { return heightMap; }
	CResource1D* GetTempHeightBuffer() { return tempHeightMap; }

private:
	ComputeShader * heightSet;
	CResource2D* heightMap;

	class Texture* texture;


	ComputeShader* computeHeightEdit;
	ComputeShader* computeCopyHeight;
	CResource1D* tempHeightMap;

};


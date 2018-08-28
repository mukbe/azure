#pragma once
class Terrain
{
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
	class TerrainHeight* heightData;
	class Shader* shader;
	class Texture* texture;
};


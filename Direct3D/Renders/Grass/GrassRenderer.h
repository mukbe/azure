#pragma once

struct VertexGrass
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 look;
	D3DXVECTOR2 scale;
	int textureID;

	VertexGrass() :pos(0.f, 0.f, 0.f), scale(0.0f,0.0f) {}
	VertexGrass(D3DXVECTOR3 pos, D3DXVECTOR3 look, D3DXVECTOR3 up, D3DXVECTOR2 scale, int texIndex)
		:pos(pos), look(look), scale(scale), textureID(texIndex) {}
};

class GrassRenderer
{
	VertexType(VertexGrass)
private:
	class Shader* shader;
	ID3D11Buffer * vertexBuffer;
	UINT maxGrassCount;
	vector<VertexGrass> grassData;
public:
	GrassRenderer(UINT maxGrassCount);
	~GrassRenderer();

	void UpdateBuffer();
	void Render();
	void UIRender();

	void AddGrass(VertexGrass grass);
private:
	void CreateBuffer(UINT grassCount);
};


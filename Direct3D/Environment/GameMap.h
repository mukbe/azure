#pragma once

class Json::Value;
class GameMap : public GameObject
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
			int Edge;
			int Inside;
			D3DXVECTOR2 padding;


		}Data;
	};

public:
	GameMap(string level = "");
	virtual~GameMap();

	virtual void Render();
	virtual void UIRender();

private:
	ID3D11Buffer * vertexBuffer;
	ID3D11Buffer* indexBuffer;

	vector<VertexTextureNormal> vertexData;
	vector<UINT> indexData;
	UINT width, height;
	UINT scale;

	Texture* heightMap;
	Texture* splatMap;
	Texture* normalMap;
	Texture* diffuseMap;
	ID3D11ShaderResourceView* splatTexView[4];

	WorldBuffer* worldBuffer;
	Shader* shader;
	Buffer* buffer;
};


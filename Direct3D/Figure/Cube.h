#pragma once
class Cube
{
private:
	ID3D11Buffer * vertexBuffer;
	ID3D11Buffer* indexBuffer;
	UINT indexCount;

	class Shader* shader;
	class WorldBuffer* worldBuffer;

	ID3D11RasterizerState* renderState;
public:
	Cube();
	~Cube();

	void Render();
};


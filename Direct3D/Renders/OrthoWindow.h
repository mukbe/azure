#pragma once
class OrthoWindow
{
private:
	VertexType(VertexTexture)
public:
	OrthoWindow(int windowWidth, int windowHeight);
	~OrthoWindow();

	void Render();

	Synthesize(ID3D11Buffer*, vertexBuffer, VertexBuffer)
	Synthesize(ID3D11Buffer*, indexBuffer, IndexBuffer)
	Synthesize(UINT, vertexCount, vertexCount)
	Synthesize(UINT, indexCount, IndexCount)

};

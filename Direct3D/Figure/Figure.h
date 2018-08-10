#pragma once
class Figure
{
private:
	using VertexType = VertexColorNormal;
public :
	enum FigureType{Box, Sphere, Grid,End};
private:
	Synthesize(class Transform*, transform, Transform)
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	UINT indexCount;
	class WorldBuffer* worldBuffer;
	class Shader* shader;
	class Shader* shadowShader;
public:
	Figure(FigureType type, float radius,D3DXCOLOR color = D3DXCOLOR(0.f,0.f,1.f,1.f));
	~Figure();

	void Render();
	void ShadowRender();
};


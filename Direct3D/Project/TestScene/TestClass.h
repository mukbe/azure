#pragma once

class TestClass
{
public:
	TestClass();
	~TestClass();

	void Update();

	void Render();
	void PostRender();


private:

	struct Line
	{
		VertexColor from, to;

		Line() {}
		Line(const VertexColor& f, const VertexColor& t) : from(f), to(t) {}
		Line(const D3DXVECTOR3& f, const D3DXVECTOR3& t, const D3DXCOLOR& fc, const D3DXCOLOR& tc)
		{
			from.position = f; from.color = fc;
			to.position = t; to.color = tc;
		}
	};
	vector<Line> lines;

	vector<VertexColor> vertexData;
	vector<UINT> indexData;

	ID3D11Buffer* vertexBuffer;

	WorldBuffer* worldbuffer;
	Shader* shader;
	D3DXMATRIX matWorld;
	D3DXMATRIX matWorld2;



	size_t testID1;
	size_t testID2;
};
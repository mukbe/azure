#pragma once

class BadyDesc;
class TestBT : public SceneNode
{
public:
	TestBT();
	~TestBT();

	virtual void Init() {}
	virtual void Update();

	virtual void Release() {}
	virtual void PreUpdate() {}
	virtual void PostUpdate() {}

	void PreRender();
	void Render();
	void PostRender();

private:
	class FreeCamera * freeCamera;

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
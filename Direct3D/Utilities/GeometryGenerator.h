#pragma once
#include <vector>
class GeometryGenerator
{
public:
	GeometryGenerator() {}
	~GeometryGenerator() {}

	struct Vertex {
		Vertex() {};
		Vertex(const D3DXVECTOR3& p, const D3DXVECTOR3& n, const D3DXVECTOR3& t, const D3DXVECTOR2& uv)
			:Position(p), Normal(n), TangentU(t), TexC(uv) {};
		Vertex(float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v)
			:Position(px, py, pz), Normal(nx, ny, nz), TangentU(tx, ty, tz), TexC(u, v) {};

		D3DXVECTOR3 Position;		//	��ǥ
		D3DXVECTOR2 TexC;			//	�ؼ� ��ǥ
		D3DXVECTOR3 Normal;			//	���
		D3DXVECTOR3 TangentU;		//	ź�� (�ؽ�ó��ǥ
	};

	struct MeshData {
		std::vector<Vertex> Vertices;
		std::vector<UINT> Indices;
	};

public:		//	������ �߽����� ����
	void CreateBox(float width, float height, float depth, MeshData& mesh);
	void CreateSphere(float radius, UINT sliceCount, UINT stackCount, MeshData& mesh);
	void CreateGrid(float width, float depth, UINT m, UINT n, MeshData& mesh);
	void CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData& mesh);
	void CreateGeosphere(float radius, UINT numSubdivisions, MeshData& mesh);

private:
	void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData& mesh);
	void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData& mesh);
	void Subdivide(MeshData& mesh);
};


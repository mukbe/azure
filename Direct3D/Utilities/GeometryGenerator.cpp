#include "stdafx.h"
#include "GeometryGenerator.h"

#include "Math.h"
void GeometryGenerator::CreateBox(float width, float height, float depth, MeshData & mesh)
{
	Vertex v[24];

	float w2 = 0.5f * width;
	float h2 = 0.5f * height;
	float d2 = 0.5f * depth;

	// Fill in the front face vertex data.
	v[0] = Vertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[1] = Vertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[2] = Vertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[3] = Vertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the back face vertex data.
	v[4] = Vertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[5] = Vertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[6] = Vertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[7] = Vertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the top face vertex data.
	v[8] = Vertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[9] = Vertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[10] = Vertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[11] = Vertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the bottom face vertex data.
	v[12] = Vertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[13] = Vertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[14] = Vertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[15] = Vertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the left face vertex data.
	v[16] = Vertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[17] = Vertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[18] = Vertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[19] = Vertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Fill in the right face vertex data.
	v[20] = Vertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[21] = Vertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[22] = Vertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	v[23] = Vertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	mesh.Vertices.assign(&v[0], &v[24]);

	UINT i[36];

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] = 9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	mesh.Indices.assign(&i[0], &i[36]);
}

void GeometryGenerator::CreateSphere(float radius, UINT sliceCount, UINT stackCount, MeshData & mesh)
{
	mesh.Indices.clear();
	mesh.Vertices.clear();

	Vertex TopVertex(0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	Vertex BottomVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

	mesh.Vertices.push_back(TopVertex);

	float PhiStep = (float)D3DX_PI / stackCount;
	float ThetaStep = 2.0f * (float)D3DX_PI / sliceCount;

	for (UINT i = 1; i <= stackCount - 1; ++i) {
		float Phi = i * PhiStep;

		for (UINT j = 0; j <= sliceCount; ++j) {
			float Theta = j * ThetaStep;

			Vertex v;
			v.Position.x = radius * sinf(Phi) * cosf(Theta);
			v.Position.y = radius * cosf(Phi);
			v.Position.z = radius * sinf(Phi) * sinf(Theta);

			v.TangentU.x = -radius * sinf(Phi) * sinf(Theta);
			v.TangentU.y = 0.0f;
			v.TangentU.z = +radius * sinf(Phi) * cosf(Theta);

			D3DXVec3Normalize(&v.TangentU, &v.TangentU);

			D3DXVec3Normalize(&v.Normal, &v.Position);

			v.TexC.x = Theta / (float)D3DX_PI;
			v.TexC.y = Phi / (float)D3DX_PI;

			mesh.Vertices.push_back(v);
		}
	}

	mesh.Vertices.push_back(BottomVertex);

	//	�ֻ��� �ε��� ����
	//	�ֻ��� ������ �� ���� ��������� ����
	for (UINT i = 1; i <= sliceCount; ++i) {
		mesh.Indices.push_back(0);
		mesh.Indices.push_back(i + 1);
		mesh.Indices.push_back(i);
	}

	UINT baseIndex = 1;
	UINT RIngVertexCount = sliceCount + 1;

	for (UINT i = 0; i < stackCount - 2; ++i) {
		for (UINT j = 0; j < sliceCount; ++j) {
			mesh.Indices.push_back(baseIndex + i * RIngVertexCount + j);
			mesh.Indices.push_back(baseIndex + i * RIngVertexCount + j + 1);
			mesh.Indices.push_back(baseIndex + (i + 1) * RIngVertexCount + j);

			mesh.Indices.push_back(baseIndex + (i + 1) * RIngVertexCount + j);
			mesh.Indices.push_back(baseIndex + i * RIngVertexCount + j + 1);
			mesh.Indices.push_back(baseIndex + (i + 1) * RIngVertexCount + j + 1);
		}
	}

	UINT SouthPoleIndex = (UINT)mesh.Vertices.size() - 1;
	baseIndex = SouthPoleIndex - RIngVertexCount;

	for (UINT i = 0; i < sliceCount; ++i) {
		mesh.Indices.push_back(SouthPoleIndex);
		mesh.Indices.push_back(baseIndex + i);
		mesh.Indices.push_back(baseIndex + i + 1);
	}

}

void GeometryGenerator::CreateGrid(float width, float depth, UINT m, UINT n, MeshData & mesh)
{
	//	������ �Ǽ� �Լ� y = f(x,y) �ϳ��� ���� (ǥ��)
	//	x, z �� �̿��� ���ڸ� ����� �� ���� �Լ�(GetHeight)�� �����ؼ� �� ǥ���� �ٻ��Ѵ�

	UINT VertexCount = m * n;
	UINT FaceCount = (m - 1) * (n - 1) * 2;		//	�� ���� �ﰢ������ ĭ(��Ʈ) ����

	float HalfWidht = 0.5f * width;
	float HalfDepth = 0.5f * depth;

	float dx = width / (n - 1);
	float dz = depth / (m - 1);

	float du = 1.0f / (n - 1);
	float dv = 1.0f / (m - 1);

	mesh.Vertices.resize(VertexCount);
	for (UINT i = 0; i < m; ++i) {
		float z = HalfDepth - i * dz;

		for (UINT j = 0; j < n; ++j) {
			float x = -HalfWidht + j * dx;

			mesh.Vertices[i * n + j].Position = D3DXVECTOR3(x, 0.0f, z);			//	��ġ
			mesh.Vertices[i * n + j].Normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);	//	����
			mesh.Vertices[i * n + j].TangentU = D3DXVECTOR3(1.0f, 0.0f, 0.0f);	//	����

			mesh.Vertices[i * n + j].TexC.x = j * du;							//	�ؽ�ó ��ǥ
			mesh.Vertices[i * n + j].TexC.y = i * dv;							//	�ؽ�ó ��ǥ
		}
	}

	mesh.Indices.resize(FaceCount * 3);
	UINT k = 0;

	//	i, j = A,	i, j + 1 = B,	i + 1,	i + 1, j = C,	i + 1, j + 1 = D
	/*
	i, j				 i, j + 1
	A	-	-	-	-	B
	|				/	|
	|			/		|
	|		/			|
	|	/				|
	|/					|
	C	-	-	-	-	D
	i + 1, j			i + 1, j + 1
	*/

	for (UINT i = 0; i < m - 1; ++i) {
		for (UINT j = 0; j < n - 1; ++j) {

			mesh.Indices[k] = i * n + j;
			mesh.Indices[k + 1] = i * n + j + 1;
			mesh.Indices[k + 2] = (i + 1) * n + j;

			mesh.Indices[k + 3] = (i + 1) * n + j;
			mesh.Indices[k + 4] = i * n + j + 1;
			mesh.Indices[k + 5] = (i + 1) * n + j + 1;

			k += 6;
		}
	}

}

void GeometryGenerator::CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData & mesh)
{
	mesh.Vertices.clear();
	mesh.Indices.clear();

	//	���� ���� ���ϰ�
	float StackHeight = height / stackCount;

	//	�� �̿��� �� ������ ������ ���� ���Ѵ� (������ ��ȭ��)
	float RadiusStep = (topRadius - bottomRadius) / stackCount;

	UINT RingCount = stackCount + 1;

	//	���ϴܺ��� �ֻ�� ��� ���� Ȯ���ϸ� �� ���� ������ ���
	for (UINT i = 0; i < RingCount; ++i) {
		float y = -0.5f * height + i * StackHeight;
		float r = bottomRadius + i * RadiusStep;

		//	���� ���ؽ�
		float Theta = 2.0f * (float)D3DX_PI / sliceCount;
		for (UINT j = 0; j <= sliceCount; ++j) {
			Vertex vertex;

			float c = cosf(j * Theta);
			float s = sinf(j * Theta);

			vertex.Position = D3DXVECTOR3(r * c, y, r * s);

			vertex.TexC.x = (float)j / sliceCount;
			vertex.TexC.y = 1.0f - (float)i / stackCount;

			vertex.TangentU = D3DXVECTOR3(-s, 0.0f, c);

			float dr = bottomRadius - topRadius;
			D3DXVECTOR3 bTangent(dr * c, -height, dr * s);

			D3DXVECTOR3 cross;
			D3DXVec3Cross(&cross, &vertex.TangentU, &bTangent);
			D3DXVec3Normalize(&vertex.Normal, &cross);

			mesh.Vertices.push_back(vertex);
		}
	}

	//	�� ������ ù ��°�� ������ ���ؽ��� �����ϱ� ������ �ϳ� �߰�
	//	+ �ؽ�ó ��ǥ�� �ٸ��� ������..
	UINT RingVertexCount = sliceCount + 1;

	//	�ε��� ���
	for (UINT i = 0; i < stackCount; ++i) {
		for (UINT j = 0; j < sliceCount; ++j) {
			mesh.Indices.push_back(i * RingVertexCount + j);
			mesh.Indices.push_back((i + 1) * RingVertexCount + j);
			mesh.Indices.push_back((i + 2) * RingVertexCount + j + 1);

			mesh.Indices.push_back(i * RingVertexCount + j);
			mesh.Indices.push_back((i + 1) * RingVertexCount + j + 1);
			mesh.Indices.push_back(i * RingVertexCount + j + 1);
		}
	}

	BuildCylinderTopCap(bottomRadius, topRadius, height, sliceCount, stackCount, mesh);
	BuildCylinderBottomCap(bottomRadius, topRadius, height, sliceCount, stackCount, mesh);
}

void GeometryGenerator::BuildCylinderTopCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData & mesh)
{
	UINT baseIndex = (UINT)mesh.Vertices.size();

	float y = 0.5f * height;
	float Theta = 2.0f * (float)D3DX_PI / sliceCount;

	//	���⼭ ����
	//	�ؽ�ó ��ǥ�� ������ �ٸ�. ���� ���� �����Ѵ�

	for (UINT i = 0; i <= sliceCount; ++i) {
		float x = topRadius * cosf(i * Theta);
		float z = topRadius * sinf(i * Theta);

		//	���̿� ���� ũ�⸦ �����Ͽ� ��� �Ѳ��� �ؽ�ó ��ǥ������ ������
		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		mesh.Vertices.push_back(Vertex(x, y, z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v));
	}

	//	�Ѳ� ���� ���ؽ�
	mesh.Vertices.push_back(Vertex(0.0f, y, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f));

	UINT centerIndex = (UINT)mesh.Vertices.size() - 1;
	for (UINT i = 0; i < sliceCount; ++i) {
		mesh.Indices.push_back(centerIndex);
		mesh.Indices.push_back(baseIndex + i + 1);
		mesh.Indices.push_back(baseIndex + i);
	}
}

void GeometryGenerator::BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData & mesh)
{
	//	�� �Ѳ��̶� �Ȱ���. ���� ���̸� �����ִ°�
	UINT baseIndex = (UINT)mesh.Vertices.size();

	float y = -0.5f * height;

	float Theta = 2.0f * (float)D3DX_PI / sliceCount;

	for (UINT i = 0; i <= sliceCount; ++i) {
		float x = bottomRadius * cosf(i * Theta);
		float z = bottomRadius * sinf(i * Theta);

		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		mesh.Vertices.push_back(Vertex(x, y, z, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v));
	}

	mesh.Vertices.push_back(Vertex(0.0f, y, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f));

	UINT centerIndex = (UINT)mesh.Vertices.size() - 1;
	for (UINT i = 0; i < sliceCount; ++i) {
		mesh.Indices.push_back(centerIndex);
		mesh.Indices.push_back(baseIndex + i);
		mesh.Indices.push_back(baseIndex + i + 1);
	}
}

void GeometryGenerator::CreateGeosphere(float radius, UINT numSubdivisions, MeshData & mesh)
{
	//	�ﰢ���� ����ȭ�ؼ� �׸���
	//	DX9 ������ �ﰢ������ ������ ����ġ ���� ���� �����Ǵµ�, ���⼭��
	//	������(������ ���� ���� ���� ���̵� ���� �ﰢ��) �� ���� �����Ѵ�

	//	����ȭ ȸ���� ����ġ�� �д�
	//numSubdivisions = Math::Min(numSubdivisions, 5u);

	//	���̽ʸ�ü�� �׼����̼��Ͽ� ���� �ٻ�
	const float X = 0.525731f;
	const float Z = 0.850651f;

	D3DXVECTOR3 pos[12] =
	{
		D3DXVECTOR3(-X, 0.0f, Z),  D3DXVECTOR3(X, 0.0f, Z),
		D3DXVECTOR3(-X, 0.0f, -Z), D3DXVECTOR3(X, 0.0f, -Z),
		D3DXVECTOR3(0.0f, Z, X),   D3DXVECTOR3(0.0f, Z, -X),
		D3DXVECTOR3(0.0f, -Z, X),  D3DXVECTOR3(0.0f, -Z, -X),
		D3DXVECTOR3(Z, X, 0.0f),   D3DXVECTOR3(-Z, X, 0.0f),
		D3DXVECTOR3(Z, -X, 0.0f),  D3DXVECTOR3(-Z, -X, 0.0f)
	};

	DWORD k[60] =
	{
		1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
		1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
	};

	mesh.Vertices.resize(12);
	mesh.Indices.resize(60);

	for (UINT i = 0; i < 12; ++i)
		mesh.Vertices[i].Position = pos[i];

	for (UINT i = 0; i < 60; ++i)
		mesh.Indices[i] = k[i];

	for (UINT i = 0; i < numSubdivisions; ++i)
		Subdivide(mesh);

	//	���ؽ��� ���� �����ϰ� ���
	for (UINT i = 0; i < mesh.Vertices.size(); ++i)
	{
		//	���� ���� ����
		D3DXVECTOR3 n;
		D3DXVec3Normalize(&n, &mesh.Vertices[i].Position);

		//	���������� ��ʽ�Ų��
		D3DXVECTOR3 p = radius * n;

		mesh.Vertices[i].Position = p;
		mesh.Vertices[i].Normal = n;

		////	���� ��ǥ�κ��� �ؽ�ó ��ǥ�� ���Ѵ�
		//float theta = Math::AngleFromXY(
		//	mesh.Vertices[i].Position.x,
		//	mesh.Vertices[i].Position.z);

		float phi = acosf(mesh.Vertices[i].Position.y / radius);

		mesh.Vertices[i].TexC.x = theta / ((float)D3DX_PI * 2.f);
		mesh.Vertices[i].TexC.y = phi / (float)D3DX_PI;

		//	��Ÿ(theta) �� �̿��� ��̺��� ���Ѵ�
		mesh.Vertices[i].TangentU.x = -radius * sinf(phi) * sinf(theta);
		mesh.Vertices[i].TangentU.y = 0.0f;
		mesh.Vertices[i].TangentU.z = +radius * sinf(phi) * cosf(theta);

		D3DXVec3Normalize(&mesh.Vertices[i].TangentU, &mesh.Vertices[i].TangentU);
	}
}

void GeometryGenerator::Subdivide(MeshData & mesh)
{
	MeshData inputCopy = mesh;

	mesh.Vertices.resize(0);
	mesh.Indices.resize(0);

	//       v1
	//       *
	//      / \
			//     /   \
	//  m0*-----*m1
//   / \   / \
	//  /   \ /   \
	// *-----*-----*
// v0    m2     v2

	UINT numTris = static_cast<UINT>(inputCopy.Indices.size() / 3);
	for (UINT i = 0; i < numTris; ++i)
	{
		Vertex v0 = inputCopy.Vertices[inputCopy.Indices[i * 3 + 0]];
		Vertex v1 = inputCopy.Vertices[inputCopy.Indices[i * 3 + 1]];
		Vertex v2 = inputCopy.Vertices[inputCopy.Indices[i * 3 + 2]];

		//	��� �� ����
		Vertex m0, m1, m2;

		m0.Position = D3DXVECTOR3(
			0.5f*(v0.Position.x + v1.Position.x),
			0.5f*(v0.Position.y + v1.Position.y),
			0.5f*(v0.Position.z + v1.Position.z));

		m1.Position = D3DXVECTOR3(
			0.5f*(v1.Position.x + v2.Position.x),
			0.5f*(v1.Position.y + v2.Position.y),
			0.5f*(v1.Position.z + v2.Position.z));

		m2.Position = D3DXVECTOR3(
			0.5f*(v0.Position.x + v2.Position.x),
			0.5f*(v0.Position.y + v2.Position.y),
			0.5f*(v0.Position.z + v2.Position.z));

		mesh.Vertices.push_back(v0); // 0
		mesh.Vertices.push_back(v1); // 1
		mesh.Vertices.push_back(v2); // 2
		mesh.Vertices.push_back(m0); // 3
		mesh.Vertices.push_back(m1); // 4
		mesh.Vertices.push_back(m2); // 5

		mesh.Indices.push_back(i * 6 + 0);
		mesh.Indices.push_back(i * 6 + 3);
		mesh.Indices.push_back(i * 6 + 5);

		mesh.Indices.push_back(i * 6 + 3);
		mesh.Indices.push_back(i * 6 + 4);
		mesh.Indices.push_back(i * 6 + 5);

		mesh.Indices.push_back(i * 6 + 5);
		mesh.Indices.push_back(i * 6 + 4);
		mesh.Indices.push_back(i * 6 + 2);

		mesh.Indices.push_back(i * 6 + 3);
		mesh.Indices.push_back(i * 6 + 1);
		mesh.Indices.push_back(i * 6 + 4);
	}
}
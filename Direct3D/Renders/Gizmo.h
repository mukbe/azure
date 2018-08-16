#pragma once
#include "DefineHeader.h"
#define CIRCLEGIZMO_SEGMENTS 36

class Gizmo 
{
	Singleton(Gizmo)
private:
	UINT vertexCount;
	UINT indexCount;

	class Shader* shader;
	class WorldBuffer* worldBuffer;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	ID3D11Buffer* circleVertexBuffer;
	ID3D11Buffer* circleIndexBuffer;

	struct VertexColor* circleVertices;

	D3DXVECTOR3 circlPosition[CIRCLEGIZMO_SEGMENTS + 1];
private:
	void CreateVertex();
	void CreateIndex();
public:

	void Line(const D3DXVECTOR3 startPos, const D3DXVECTOR3 endPos, const D3DXCOLOR color);
	void Circle(D3DXVECTOR3 center, float radius, D3DXVECTOR3 axis, D3DXCOLOR color);
	void WireSphere(D3DXVECTOR3 center, float radius, D3DXCOLOR color);
	void AABB(const D3DXVECTOR3 minPos, const D3DXVECTOR3 maxPos, const D3DXCOLOR color);
	void OBB(vector<D3DXVECTOR3>& corners, const D3DXCOLOR color);
	void LocalGizmo(D3DXVECTOR3 center, float radius, D3DXVECTOR3 right, D3DXVECTOR3 up, D3DXVECTOR3 forward);
};


#define GizmoRenderer Gizmo::GetInstance()
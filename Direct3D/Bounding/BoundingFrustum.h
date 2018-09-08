#pragma once

#define PLANE_EPSILON 5.0f

class BoundingFrustum
{
public:
	//absolute
	D3DXVECTOR3			vertex[8];
	//to world
	D3DXVECTOR3			transformVertex[8];
	D3DXPLANE			plane[6];
public:
	BoundingFrustum();
	~BoundingFrustum();

private:
	void CreatePlane(LPD3DXPLANE pOut, const D3DXVECTOR3* v0, const D3DXVECTOR3* v1, const D3DXVECTOR3* v2);
	float PlaneDot(const LPD3DXPLANE plane, const D3DXVECTOR3* point);
public:
	bool IsPointInFrustum(D3DXVECTOR3* p);
	bool IsAABBInFrustum(class BoundingBox* boundingBox);
	bool IsSphereInFrustum(D3DXVECTOR3* p, float radius);
	bool IsSphereInFrustum(class BoundingBox* boundingBox);

	void UpdateFrustum(D3DXMATRIX viewProj);
	void Render();
};


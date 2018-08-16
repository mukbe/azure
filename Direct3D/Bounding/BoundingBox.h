#pragma once
class BoundingBox
{
public:
	D3DXVECTOR3 minPos;
	D3DXVECTOR3 maxPos;
	D3DXVECTOR3 halfSize;
public:
	BoundingBox();
	BoundingBox(D3DXVECTOR3 min, D3DXVECTOR3 max);
	BoundingBox(class BoundingSphere sphere);

	void Render(D3DXMATRIX matWorld,bool DrawAABB = true);

	void GetCorners(D3DXVECTOR3* pOut);
	void GetCorners(vector<D3DXVECTOR3>& pOut);
	void GetCorners(vector<D3DXVECTOR3>& pOut, D3DXMATRIX mat);
	
	bool IntersectsAABB(class BoundingBox box);
	bool Intersects(class BoundingFrustum frustum);
	enum PlaneIntersectionType Intersects(D3DXPLANE plane);
	bool Intersects(class Ray ray, float* result);
	bool Intersects(BoundingSphere sphere);

	static bool IntersectsOBB(class Transform * pTransA, class BoundingBox * pBoundA, class Transform * pTransB, class BoundingBox * pBoundB);
};


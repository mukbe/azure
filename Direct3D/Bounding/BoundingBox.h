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

	void Render(D3DXMATRIX matWorld,bool DrawAABB = true,D3DXCOLOR color = D3DXCOLOR(1.f,0.f,0.f,1.f));
	void RenderAABB(D3DXCOLOR color = D3DXCOLOR(1.f, 0.f, 0.f, 1.f));

	void GetCorners(D3DXVECTOR3* pOut);
	void GetCorners(vector<D3DXVECTOR3>& pOut);
	void GetCorners(vector<D3DXVECTOR3>& pOut, D3DXMATRIX mat);
	void GetCenterAndRadius(D3DXVECTOR3* pOutCenter,float* pOutRadius);
	
	bool IntersectsAABB(class BoundingBox box);
	bool Intersects(class BoundingFrustum frustum);
	enum PlaneIntersectionType Intersects(D3DXPLANE plane);
	bool Intersects(class Ray ray, float* result);
	bool Intersects(class BoundingSphere sphere);

	static bool IntersectsOBB(class Transform * pTransA, class BoundingBox * pBoundA, class Transform * pTransB, class BoundingBox * pBoundB);
	static bool IntersectsOBB(D3DXMATRIX matA, class BoundingBox* pBoundA, D3DXMATRIX matB, class BoundingBox* pBoundB);
};


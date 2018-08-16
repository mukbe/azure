#pragma once
class BoundingBox
{
public:
	D3DXVECTOR3 minPos;
	D3DXVECTOR3 maxPos;
public:
	BoundingBox();
	BoundingBox(D3DXVECTOR3 min, D3DXVECTOR3 max);
	BoundingBox(class BoundingSphere sphere);

	void GetCorners(D3DXVECTOR3* pOut);
	
	bool IntersectsAABB(class BoundingBox box);
	bool Intersects(class BoundingFrustum frustum);
	enum PlaneIntersectionType Intersects(D3DXPLANE plane);
	bool Intersects(class Ray ray, float* result);
	bool Intersects(BoundingSphere sphere);

	class BoundingBox Transform(D3DXMATRIX mat);
};


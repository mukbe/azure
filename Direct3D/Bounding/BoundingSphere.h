#pragma once

class BoundingSphere
{
public:
	D3DXVECTOR3 center;
	float radius;
public:
	BoundingSphere(void);
	BoundingSphere(D3DXVECTOR3 center, float radius);
	BoundingSphere(class BoundingBox box);
	
	bool Intersects(class BoundingBox box);
	bool Intersects(class BoundingFrustum frustum);
	enum PlaneIntersectionType Intersects(D3DXPLANE plane);
	bool Intersects(class Ray ray, float* result);
	bool Intersects(class BoundingSphere sphere);

	BoundingSphere Transform(D3DXMATRIX mat);
};


#pragma once
class Ray
{
public:
	Ray(void);
	Ray(D3DXVECTOR3 position, D3DXVECTOR3 direction);

	bool Intersects(D3DXPLANE& plane, float* value);
	bool Intersects(class BoundingSphere& sphere, float* value);

public:
	D3DXVECTOR3 origin;
	D3DXVECTOR3 direction;
};


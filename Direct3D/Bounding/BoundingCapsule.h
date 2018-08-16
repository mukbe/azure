#pragma once
class BoundingCapsule
{
public:
	BoundingCapsule();
	BoundingCapsule(D3DXVECTOR3 center, D3DXVECTOR3 direction, float height, float radius);

	bool Intersects(const BoundingCapsule& capsule) const;
	BoundingCapsule Transform(D3DXMATRIX mat);
private:
	bool CheckBetweenLines(const D3DXVECTOR3& line1, const D3DXVECTOR3& line2, float length) const;
public:
	D3DXVECTOR3 center; //원래의 값
	D3DXVECTOR3 direction;

	float height;
	float radius;
};


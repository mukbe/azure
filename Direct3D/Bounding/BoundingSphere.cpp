#include "stdafx.h"
#include "BoundingSphere.h"

#include "Ray.h"
#include "BoundingBox.h"
#include "BoundingCapsule.h"
#include "BoundingFrustum.h"
#include "../Utilities/Math.h"


BoundingSphere::BoundingSphere()
	:center(0.f,0.f,0.f), radius(0.f)
{
}

BoundingSphere::BoundingSphere(D3DXVECTOR3 center, float radius)
	:center(center),radius(radius)
{
}

BoundingSphere::BoundingSphere(BoundingBox box)
{
	D3DXVec3Lerp(&this->center, &box.minPos, &box.maxPos, 0.5f);

	float result = D3DXVec3Length(&(box.minPos - box.maxPos));
	this->radius = result * 0.5f;
}

bool BoundingSphere::Intersects(BoundingBox box)
{
	D3DXVECTOR3 result1;
	result1.x = Math::Clamp(center.x, box.minPos.x, box.maxPos.x);
	result1.y = Math::Clamp(center.y, box.minPos.y, box.maxPos.y);
	result1.z = Math::Clamp(center.z, box.minPos.z, box.maxPos.z);

	float current0, current1, current2;
	current0 = center.x - result1.x;
	current1 = center.y - result1.y;
	current2 = center.z - result1.z;

	float result2 = (float)((double)current0 * (double)current0 + (double)current1 * (double)current1 + (double)current2 * (double)current2);

	if ((double)result2 <= (double)radius * (double)radius)
		return true;
	else
		return false;
}

bool BoundingSphere::Intersects(BoundingFrustum frustum)
{
	return false;
}

PlaneIntersectionType BoundingSphere::Intersects(D3DXPLANE plane)
{
	return PlaneIntersectionType();
}

bool BoundingSphere::Intersects(Ray ray, float * result)
{
	return ray.Intersects(*this, result);
}

bool BoundingSphere::Intersects(BoundingSphere sphere)
{
	float result = D3DXVec3LengthSq(&(this->center - sphere.center));
	float num1 = radius;
	float num2 = sphere.radius;

	if ((double)num1 * (double)num1 + 2.0 * (double)num1 * (double)num2 + (double)num2 * (double)num2 <= (double)result)
		return false;
	else
		return true;
}

BoundingSphere BoundingSphere::Transform(D3DXMATRIX mat)
{
	BoundingSphere boundingSphere;
	D3DXVec3TransformCoord(&boundingSphere.center, &this->center, &mat);

	float num = Math::Max((float)(
		(double)mat._11 * (double)mat._11 + (double)mat._12 * (double)mat._12 + (double)mat._13 * (double)mat._13),
		Math::Max((float)((double)mat._21 * (double)mat._21 + (double)mat._22 * (double)mat._22 + (double)mat._23 * (double)mat._23),
		(float)((double)mat._31 * (double)mat._31 + (double)mat._32 * (double)mat._32 + (double)mat._33 * (double)mat._33)));

	boundingSphere.radius = this->radius * (float)sqrt((double)num);

	return boundingSphere;
}




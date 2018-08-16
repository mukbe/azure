#include "stdafx.h"
#include "BoundingBox.h"

#include "Ray.h"
#include "BoundingSphere.h"
#include "BoundingCapsule.h"
#include "BoundingFrustum.h"
#include "../Utilities/Math.h"


BoundingBox::BoundingBox()
	:minPos(0.f,0.f,0.f),maxPos(0.f,0.f,0.f)
{
}

BoundingBox::BoundingBox(D3DXVECTOR3 min, D3DXVECTOR3 max)
	:minPos(min),maxPos(max)
{
}

BoundingBox::BoundingBox(BoundingSphere sphere)
{
	this->minPos.x = sphere.center.x - sphere.radius;
	this->minPos.y = sphere.center.y - sphere.radius;
	this->minPos.z = sphere.center.z - sphere.radius;
	this->maxPos.x = sphere.center.x + sphere.radius;
	this->maxPos.y = sphere.center.y + sphere.radius;
	this->maxPos.z = sphere.center.z + sphere.radius;
}

void BoundingBox::GetCorners(D3DXVECTOR3 * pOut)
{
	pOut[0] = D3DXVECTOR3(minPos.x, maxPos.y, maxPos.z);
	pOut[1] = D3DXVECTOR3(maxPos.x, maxPos.y, maxPos.z);
	pOut[2] = D3DXVECTOR3(maxPos.x, minPos.y, maxPos.z);
	pOut[3] = D3DXVECTOR3(minPos.x, minPos.y, maxPos.z);
	pOut[4] = D3DXVECTOR3(minPos.x, maxPos.y, minPos.z);
	pOut[5] = D3DXVECTOR3(maxPos.x, maxPos.y, minPos.z);
	pOut[6] = D3DXVECTOR3(maxPos.x, minPos.y, minPos.z);
	pOut[7] = D3DXVECTOR3(minPos.x, minPos.y, minPos.z);
}

bool BoundingBox::IntersectsAABB(BoundingBox box)
{
	if ((double)maxPos.x < (double)box.minPos.x || (double)minPos.x >(double)box.maxPos.x || ((double)maxPos.y < (double)box.minPos.y
		|| (double)minPos.y >(double)box.maxPos.y) || ((double)maxPos.z < (double)box.minPos.z || (double)minPos.z >(double)box.maxPos.z))
		return false;
	else
		return true;
}

bool BoundingBox::Intersects(BoundingFrustum frustum)
{
	return false;
}

PlaneIntersectionType BoundingBox::Intersects(D3DXPLANE plane)
{
	return PlaneIntersectionType();
}

bool BoundingBox::Intersects(Ray ray, float * result)
{
	if (result != NULL)
	{
		delete result;
		result = NULL;
	}

	float num1 = 0.0f;
	float num2 = Math::FloatMax;


	//1. ray.Direction.X
	if ((double)Math::Abs(ray.direction.x) < 9.99999997475243E-07)
	{
		if ((double)ray.origin.x < (double)minPos.x || (double)ray.origin.x >(double)maxPos.x)
			return false;
	}
	else
	{
		float num3 = 1.0f / ray.direction.x;
		float num4 = (minPos.x - ray.origin.x)* num3;
		float num5 = (maxPos.x - ray.origin.x)* num3;

		if ((double)num4 > (double)num5)
		{
			float num6 = num4;
			num4 = num5;
			num5 = num6;
		}

		num1 = Math::Max(num4, num1);
		num2 = Math::Min(num5, num2);

		if ((double)num1 > (double)num2)
			return false;
	}


	//2. ray.Direction.Y
	if ((double)Math::Abs(ray.direction.y) < 9.99999997475243E-07)
	{
		if ((double)ray.origin.y < (double)minPos.y || (double)ray.origin.y >(double)maxPos.y)
			return false;
	}
	else
	{
		float num3 = 1.0f / ray.direction.y;
		float num4 = (minPos.y - ray.origin.y) * num3;
		float num5 = (maxPos.y - ray.origin.y) * num3;

		if ((double)num4 > (double)num5)
		{
			float num6 = num4;
			num4 = num5;
			num5 = num6;
		}

		num1 = Math::Max(num4, num1);
		num2 = Math::Min(num5, num2);

		if ((double)num1 > (double)num2)
			return false;
	}


	//3. ray.Direction.Z
	if ((double)Math::Abs(ray.direction.z) < 9.99999997475243E-07)
	{
		if ((double)ray.origin.z < (double)minPos.z || (double)ray.origin.z >(double)maxPos.Z)
			return false;
	}
	else
	{
		float num3 = 1.0f / ray.direction.z;
		float num4 = (minPos.z - ray.origin.z) * num3;
		float num5 = (maxPos.z - ray.origin.z) * num3;

		if ((double)num4 > (double)num5)
		{
			float num6 = num4;
			num4 = num5;
			num5 = num6;
		}

		num1 = Math::Max(num4, num1);
		float num7 = Math::Min(num5, num2);

		if ((double)num1 > (double)num7)
			return false;
	}

	result = new float(num1);
	return true;
}

bool BoundingBox::Intersects(BoundingSphere sphere)
{
	D3DXVECTOR3 result1;
	result1.x = Math::Clamp(sphere.center.x, minPos.x, maxPos.x);
	result1.y = Math::Clamp(sphere.center.y, minPos.y, maxPos.y);
	result1.z = Math::Clamp(sphere.center.z, minPos.z, maxPos.z);

	float result2 = D3DXVec3LengthSq(&(sphere.center - result1));// Vector3::DistanceSquared(sphere.Center, result1);

	if ((double)result2 <= (double)sphere.radius * (double)sphere.radius)
		return true;
	else
		return false;
}

BoundingBox BoundingBox::Transform(D3DXMATRIX mat)
{

	return BoundingBox();
}


